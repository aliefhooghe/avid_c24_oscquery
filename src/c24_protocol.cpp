

#include "c24_protocol.hpp"
#include "c24_parameter.hpp"


using namespace ossia;
using namespace device;

namespace c24_device {
    
    ////

    digidesign_c24_protocol::digidesign_c24_protocol()
    {
        m_c24_surface = c24_surface_open(1024);

        if (m_c24_surface == nullptr)
            throw std::runtime_error("digidesign c24 surface open " + std::string{strerror(errno)});

        c24_surface_set_user_data(m_c24_surface, this);
        c24_surface_set_button_callback(m_c24_surface, button_event_callback);
        c24_surface_set_slider_callback(m_c24_surface, slider_move_callback);
        c24_surface_set_knob_callback(m_c24_surface, knob_rotate_callback);
        c24_surface_set_reconnection_callback(m_c24_surface, reconnection_callback);
    }

    digidesign_c24_protocol::~digidesign_c24_protocol()
    {
        c24_surface_close(m_c24_surface);
    } 

    void digidesign_c24_protocol::set_device(ossia::net::device_base& dev)
    {
        std::array<std::string, C24_TRACK_COUNT> track_prefix;

        m_device = &dev;
        auto& root_node = m_device->get_root_node();

        //  Prepare track_prefixs and Create Sliders , PanKnobs, and vumeters
        
        for (uint8_t track_id = 0; track_id < 24; ++track_id) {
            
            track_prefix[track_id] = "/Channel-" + std::to_string(track_id + 1) + "/";

            m_slider_parameters[track_id] = 
                device_parameter::create_device_parameter<c24_slider_parameter>(
                    root_node, track_prefix[track_id] + "slider/", 
                        0.0, m_c24_surface, track_id);

            m_pan_knob_parameters[track_id] = 
                device_parameter::create_device_parameter<c24_pan_knob_parameter>(
                    root_node, track_prefix[track_id] + "panKnob/", 
                        0.0, m_c24_surface, track_id);

            m_display_parameters[track_id] =
                device_parameter::create_device_parameter<c24_display_parameter>(
                    root_node, track_prefix[track_id] + "trackName/", "ch-" + 
                        std::to_string(track_id), m_c24_surface, track_id);

            // Vumeters parameters
            device_parameter::create_device_parameter<c24_vumeter_parameter>(
                root_node,  track_prefix[track_id] + "vuMeter1/", 
                    0.0, m_c24_surface, track_id, 0);

            device_parameter::create_device_parameter<c24_vumeter_parameter>(
                root_node,  track_prefix[track_id] + "vuMeter2/", 
                    0.0, m_c24_surface, track_id, 1);
        }

        //  Create PitchWheel

        m_pitch_wheel_parameter = 
            device_parameter::create_device_parameter(
                root_node, "/pitchWheel", 0.0,
                val_type::FLOAT,
                bounding_mode::FREE,
                access_mode::GET,
                init_domain(val_type::FLOAT));

        //  Creates Buttons

        for(uint16_t button = 0; button < C24_BUTTON_COUNT; button++) {
            
            const uint8_t track_id = 
                c24_button_track(button);

            std::string path{};

            if (track_id < C24_TRACK_COUNT)  //  The button is associated with a track
                path = track_prefix[track_id] + std::string{c24_surface_get_button_name(button)};
            else
                path = "/" + std::string{c24_surface_get_button_name(button)};


            m_button_parameters[button] = 
                    device_parameter::create_device_parameter<c24_button_parameter>(
                        root_node, path, false, m_c24_surface, button);
        }


    }

    bool digidesign_c24_protocol::pull(net::parameter_base& param)
    {
        return true;
    }

    bool digidesign_c24_protocol::push(const net::parameter_base& param, const ossia::value&)
    {
        return true;
    }

    bool digidesign_c24_protocol::observe(net::parameter_base& param, bool enable)
    {
        return false;
    }

    bool digidesign_c24_protocol::push_raw(const ossia::net::full_parameter_data& data)
    {
        return false;
    }

    bool digidesign_c24_protocol::update(ossia::net::node_base&)
    {
        return true;
    }

    /*
    *       C24 Driver Callbacks  
    */

    void digidesign_c24_protocol::button_event_callback(void *user_data, const uint16_t button, const uint8_t state)
    {
        digidesign_c24_protocol *self = (digidesign_c24_protocol*)user_data;
        auto& param = *(self->m_button_parameters[button]);           

        //  
        if (c24_button_is_slider(button)) {   

            if (!state) {   // erase display when release a slider
                const uint8_t track_id = c24_button_track(button);
                c24_surface_display(self->m_c24_surface, "", track_id, 1);   //  erase
            }
            param.push_value(static_cast<bool>(state));
        }
        else if (state) {
            //  Toggle for other buttons
            const bool old_value = param.value().get<bool>();
            param.push_value(!old_value);
        } 

    }

    void digidesign_c24_protocol::knob_rotate_callback(
        void *user_data, const uint16_t knob, const uint8_t state)
    {
        digidesign_c24_protocol *self = (digidesign_c24_protocol*)user_data;

        const float factor = 0.025;
        const float delta = factor * (static_cast<float>(state) - 64.0f);
    
        if ( c24_knob_is_pan_knob(knob)) {
            const uint8_t track_id = c24_knob_track(knob);
            const float old_value = self->m_pan_knob_parameters[track_id]->value().get<float>();
            float new_value = old_value + delta;

            if (new_value < -1.0)
                new_value = -1.0;
            else if (new_value > 1.0)
                new_value = 1.0;
            
            self->m_pan_knob_parameters[track_id]->push_value(new_value);
        }
        else if(knob == C24_PITCH_WHEEL) {
            const float old_value = self->m_pitch_wheel_parameter->value().get<float>();
            self->m_pitch_wheel_parameter->push_value(old_value + delta);
        }
        
    }

    void digidesign_c24_protocol::slider_move_callback(void *user_data, const uint8_t track_id, const uint16_t value)
    {
        digidesign_c24_protocol *self = (digidesign_c24_protocol*)user_data;
        const float new_value = static_cast<float>(value) / 1023.0;
        c24_surface_display_float(self->m_c24_surface, new_value, track_id, 1);
        self->m_slider_parameters[track_id]->device_value_change_event(new_value);  // feedback managed by the driver
    }

    void digidesign_c24_protocol::reconnection_callback(void *user_data)
    {
        digidesign_c24_protocol *self = (digidesign_c24_protocol*)user_data;

        for (unsigned int i = 0; i < C24_TRACK_COUNT; ++i) {
            self->m_slider_parameters[i]->device_update_value();
            self->m_pan_knob_parameters[i]->device_update_value();
            self->m_display_parameters[i]->device_update_value();
        }

        for(auto& button_param : self->m_button_parameters)
            button_param->device_update_value();
    }
}

