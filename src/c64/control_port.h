//
// Created by frank on 19.10.22.
//

#ifndef PICOC64USB_CONTROL_PORT_H
#define PICOC64USB_CONTROL_PORT_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool up : 1;
    bool down : 1;
    bool left : 1;
    bool right : 1;

    bool fire : 1;
    bool r0 : 1;
    bool r1: 1;
    bool r2: 1;

    uint8_t pot_x;
    uint8_t pot_y;
} c64_control_port_t;

#ifdef __cplusplus
}
#endif
#endif //PICOC64USB_CONTROL_PORT_H
