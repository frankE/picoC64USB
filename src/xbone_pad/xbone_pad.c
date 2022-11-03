#include "class/xbone_pad/xbone_pad_host.h"
#include "config.h"
#include "c64/control_port.h"
#include "debug.h"
#include "main.h"

inline bool trigger_to_digital(uint16_t value) {
    return value > TRIGGER_DIGITAL_THRESHOLD;
}

inline bool stick_axis_to_digital(int32_t value) {
    return value > AXIS_DIGITAL_THRESHOLD;
}

void xbone_pad_to_joystick(xbone_pad_input_t *input, c64_control_port_t *port) {
    port->fire = JOYSTICK_FIRE;
    port->up = JOYSTICK_UP;
    port->down = JOYSTICK_DOWN;
    port->left = JOYSTICK_LEFT;
    port->right = JOYSTICK_RIGHT;
    port->pot_x = JOYSTICK_PADDLE_X;
    port->pot_y = JOYSTICK_PADDLE_Y;
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+
void xbone_pad_mounted_cb(uint8_t device, uint8_t interface) {
    register_device(device, interface, XBONE_GAMEPAD);
}

void xbone_pad_removed_cb(uint8_t device) {
    remove_device(device);
}

void xbone_pad_input_event_cb(uint8_t dev_addr, uint8_t interface, uint8_t *data, uint16_t len) {
    LOG_TRACE("Input event for device %u received", dev_addr);
    if (len < sizeof(xbone_pad_input_t) || data[0] != GIP_CMD_INPUT)
        return;
    device_t *device = find_device(dev_addr);
    if (!device)
        return;
    xbone_pad_input_t *input;
    input = (xbone_pad_input_t *) data;
    c64_control_port_t control_port;
    xbone_pad_to_joystick(input, &control_port);
    write_port_state(device, &control_port);
    if ((JOYSTICK_AUTOFIRE) && !control_port.fire) {
        if (device->autofire_state == 0)
            device->autofire_state = 1;
    } else {
        device->autofire_state = 0;
    }
    if (JOYSTICK_SWAP_PORTS) {
        swap_ports(device);
    }
}
