#include "tusb.h"
#include "main.h"
#include "debug.h"

void hid_mouse_to_control_port(const hid_mouse_report_t *input, c64_control_port_t *port) {
    // todo: find out how to convert paddle data
    port->pot_x = input->x;
    port->pot_y = input->y;
    port->fire = input->buttons & MOUSE_BUTTON_LEFT;
    port->up =  input->buttons & MOUSE_BUTTON_RIGHT;
    port->down = false;
    port->left = false;
    port->right = false;
}

void handle_mouse(device_t *device, uint8_t const *report, uint16_t len) {
    const hid_mouse_report_t *mouse = (hid_mouse_report_t *)report;
    c64_control_port_t port;
    hid_mouse_to_control_port(mouse, &port);
    write_port_state(device, &port);
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    if (tuh_hid_interface_protocol(dev_addr, instance) != HID_ITF_PROTOCOL_MOUSE)
        return;
    register_device(dev_addr, instance, HID_MOUSE);

    // request to receive report
    // tuh_hid_report_received_cb() will be invoked when report is available
    if ( !tuh_hid_receive_report(dev_addr, instance) ) {
        printf("Error: cannot request to receive report\r\n");
    }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    remove_device(dev_addr);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    device_t *device = find_device(dev_addr);
    if (device->type == HID_MOUSE) {
        handle_mouse(device, report, len);
    }
    LOG_INFO("Mouse report received from interface %i", instance);
    if ( !tuh_hid_receive_report(dev_addr, instance) ) {
        printf("Error: cannot request to receive report\r\n");
    }
}
