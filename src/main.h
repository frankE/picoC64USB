//
// Created by frank on 22.10.22.
//

#ifndef PICOC64USB_MAIN_H
#define PICOC64USB_MAIN_H
#include "c64/control_port.h"

typedef enum {
    NONE = 0,
    XBONE_GAMEPAD,
    HID_MOUSE,
} device_type_t;

typedef enum {
    JOYSTICK,
    MOUSE,
    PADDLE,
} device_mode_t;

typedef struct {
    uint8_t fire;
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
    uint8_t pot_x;
    uint8_t pot_y;
} pin_map_t;

typedef struct {
    uint8_t device;
    uint8_t interface;
    device_type_t type;
    int8_t port;
    device_mode_t mode;
    uint8_t autofire_state;
    uint32_t last_autofire;
} device_t;

device_t *find_device(uint8_t device_address);
device_t *find_free_device_slot();

bool register_device(uint8_t dev_addr, uint8_t interface, device_type_t type);
void remove_device(uint8_t dev_addr);
void write_port_state(device_t *device, c64_control_port_t *state);
void swap_ports(device_t *device);

#endif //PICOC64USB_MAIN_H
