
#pragma once

#include <cstdint>
#include "device_parameter.hpp"


extern "C" {
#include <c24_surface.h>
}

namespace c24_device {


    class c24_slider_parameter : public device::device_parameter {

        public:
            c24_slider_parameter(
                ossia::net::node_base& node,
                struct c24_surface_t *surface,
                const uint8_t track_id);

            void device_update_value() override;

        private:
            const uint8_t m_track_id;
            struct c24_surface_t *m_surface{};
    };

    class c24_pan_knob_parameter : public device::device_parameter {

        public:
            c24_pan_knob_parameter(
                ossia::net::node_base& node,
                struct c24_surface_t *surface,
                const uint8_t track_id);

            void device_update_value() override;

        private:
            const uint8_t m_track_id;
            struct c24_surface_t *m_surface{};
    };

    class c24_button_parameter : public device::device_parameter {

        public:
            c24_button_parameter(
                ossia::net::node_base& node,
                struct c24_surface_t *surface,
                const uint16_t button_id);

            void device_update_value() override;
        
        private:
            const uint16_t m_button_id;
            struct c24_surface_t *m_surface{};
    };

    class c24_display_parameter : public device::device_parameter {

        public:
            c24_display_parameter(
                ossia::net::node_base& node,
                struct c24_surface_t *surface,
                const uint8_t track_id);

            void device_update_value() override;

        private:
            const uint8_t m_track_id;
            struct c24_surface_t *m_surface{};
    };

    class c24_vumeter_parameter : public device::device_parameter {

        public:
            c24_vumeter_parameter(
                ossia::net::node_base& node,
                struct c24_surface_t *surface,
                const uint8_t track_id,
                const uint8_t vumeter_id);

            void device_update_value() override;

        private:
            const uint8_t m_track_id;
            const uint8_t m_vumeter_id;
            struct c24_surface_t *m_surface{};
    };

}