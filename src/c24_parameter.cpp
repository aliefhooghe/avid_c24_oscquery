
#include "c24_parameter.hpp"

using namespace ossia;

namespace c24_device {

    c24_slider_parameter::c24_slider_parameter(
        net::node_base& node,
        struct c24_surface_t *surface,
        const uint8_t track_id)
        : device_parameter(
            node, 
            val_type::FLOAT,
            bounding_mode::CLIP,
            access_mode::BI, 
            make_domain(0.0, 1.0)),
            m_track_id(track_id),
            m_surface(surface)
    {
    }

    void c24_slider_parameter::device_update_value()
    {
        // [0, 1] -> [0, 255]
        const int v = 
            static_cast<int>(1023.0f * m_current_value.get<float>());
        uint16_t safe_value;

        if (v < 0)
            safe_value = 0u;
        else if (v > 1023u)
            safe_value = 1023u;
        else
            safe_value = v;

        c24_surface_set_slider_pos(
            m_surface,
            m_track_id,
            safe_value);
    }

    ////////////////////

    c24_pan_knob_parameter::c24_pan_knob_parameter(
        net::node_base& node,
        struct c24_surface_t *surface,
        const uint8_t track_id)
        : device_parameter(
            node, 
            val_type::FLOAT,
            bounding_mode::CLIP,
            access_mode::BI,
            make_domain(-1.0, 1.0)),
            m_track_id(track_id),
            m_surface(surface)
    {
    }

    void c24_pan_knob_parameter::device_update_value()
    {
        // [-1, 1] -> [0, 10] because there are 11 led per pan knob
        const int v = 
            static_cast<int>(5.5f * (m_current_value.get<float>() + 1.0f));
        uint8_t bit_shift;
        
        if (v < 0)
            bit_shift = 0u;
        else if (v > 10)
            bit_shift = 10u;
        else
            bit_shift = v;

        c24_surface_set_knob_led_mask(
            m_surface,
            m_track_id,
            0x8000 >> bit_shift
        );
    }

    ////////////////////

    c24_button_parameter::c24_button_parameter(
        net::node_base& node,
        struct c24_surface_t *surface,
        const uint16_t button_id)
    : device_parameter(
        node, val_type::BOOL,
        bounding_mode::CLIP,
        c24_button_has_led(button_id) ? access_mode::BI : access_mode::GET, 
        init_domain(val_type::BOOL)),
        m_button_id(button_id),
        m_surface(surface)
    {
    }

    void c24_button_parameter::device_update_value()
    {
        const bool v = m_current_value.get<bool>();
        c24_surface_set_button_led_state(
            m_surface,
            m_button_id,
            static_cast<uint8_t>(v));
    }

    //////////////////////

    c24_display_parameter::c24_display_parameter(
        net::node_base& node,
        struct c24_surface_t *surface,
        const uint8_t track_id)
    : device_parameter(
        node, val_type::STRING,
        bounding_mode::FREE,
        access_mode::SET,
        init_domain(val_type::STRING)),
        m_track_id(track_id),
        m_surface(surface)
    {
    }

    void c24_display_parameter::device_update_value()
    {
        const std::string& v = m_current_value.get<std::string>();
        c24_surface_display(
            m_surface,
            v.c_str(),
            m_track_id, 
            0); //  On the first line
    }


    //////////////////////

    c24_vumeter_parameter::c24_vumeter_parameter(
        net::node_base& node,
        struct c24_surface_t *surface,
        const uint8_t track_id,
        const uint8_t vumeter_id)
    : device_parameter(
        node, val_type::FLOAT,
        bounding_mode::CLIP,
        access_mode::SET,
        make_domain(0.0, 1.0)),
        m_track_id(track_id),
        m_vumeter_id(vumeter_id),
        m_surface(surface)
    {
    }

    void c24_vumeter_parameter::device_update_value()
    {
        const int v =
            static_cast<int>(14.0 * (1.0 - m_current_value.get<float>()));
        uint8_t bit_shift;

        if (v < 0)
            bit_shift = 0;
        else if (v > 14)
            bit_shift = 14;
        else
            bit_shift = v;

        c24_surface_set_vumeter_mask(
            m_surface,
            m_track_id,
            m_vumeter_id,
            (0xffff >> bit_shift));
    }

}