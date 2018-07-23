
#pragma once

#include <cstdint>
#include <array>
#include <map>

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#include "c24_parameter.hpp"

extern "C" {
#include <c24_surface.h>
}


namespace c24_device {

    class OSSIA_EXPORT digidesign_c24_protocol final :
        public ossia::net::protocol_base
    {
        public:
            digidesign_c24_protocol();
            ~digidesign_c24_protocol();

            void set_device(ossia::net::device_base& dev) override;

            bool pull(ossia::net::parameter_base& param) override;
            bool push(const ossia::net::parameter_base& param) override;
            bool push_raw(const ossia::net::full_parameter_data&) override;
            bool observe(ossia::net::parameter_base& param, bool enable) override;
    
            bool update(ossia::net::node_base&) override;
        
        private:
            static void button_event_callback(void *user_data, const uint16_t button, const uint8_t state);
            static void knob_rotate_callback(void *user_data, const uint16_t knob, const uint8_t state);
            static void slider_move_callback(void *user_data, const uint8_t track_id, const uint16_t value);
            static void reconnection_callback(void *user_data);

            std::array<device::device_parameter*, C24_TRACK_COUNT> m_slider_parameters;
            std::array<device::device_parameter*, C24_TRACK_COUNT> m_pan_knob_parameters;
            std::array<device::device_parameter*, C24_TRACK_COUNT> m_display_parameters;

            std::array<device::device_parameter*, C24_BUTTON_COUNT> m_button_parameters;
            device::device_parameter* m_pitch_wheel_parameter;

            ossia::net::device_base* m_device{};
            struct c24_surface_t *m_c24_surface{};
    };

}