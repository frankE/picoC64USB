#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/stdio_uart.h>
#include <hardware/pwm.h>
#include "tusb.h"
#include "config.h"
#include "c64/control_port.h"
#include "pins.h"
#include "debug.h"
#include "main.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

// 1 - 4
#define MAX_DEVICES 1
#define NUM_PORTS   1

static const pin_map_t pin_map[NUM_PORTS] = {
    {PORT1_PIN_FIRE, PORT1_PIN_UP, PORT1_PIN_DOWN, PORT1_PIN_LEFT, PORT1_PIN_RIGHT, PORT1_PIN_POT_X, PORT1_PIN_POT_Y},
#if NUM_PORTS > 1
    {PORT2_PIN_FIRE, PORT2_PIN_UP, PORT2_PIN_DOWN, PORT2_PIN_LEFT, PORT2_PIN_RIGHT, PORT2_PIN_POT_X, PORT2_PIN_POT_Y},
#endif
};

device_t device_map[MAX_DEVICES] = {};

inline device_t *find_device(uint8_t device_address) {
    for (int8_t i=0; i < MAX_DEVICES; i++) {
        if (device_map[i].device == device_address)
            return &device_map[i];
    }
    return NULL;
}

inline device_t *find_free_device_slot() {
    for (int8_t i=0; i < MAX_DEVICES; i++) {
        if (device_map[i].type == NONE)
            return &device_map[i];
    }
    return NULL;
}

inline void set_digital_pin(uint8_t pin, bool value) {
#ifdef DIGITAL_OUTPUTS_AS_OPEN_DRAIN
    gpio_set_dir(pin, value);
#else
#ifdef INVERT_DIGITAL_OUTPUTS
    gpio_put(pin, !value);
#else
    gpio_put(pin, value);
#endif
#endif
}

void set_analog_pin(uint8_t pin, uint8_t value) {
    pwm_set_gpio_level(pin, value);
}

void port_write(const pin_map_t *map, c64_control_port_t *port) {
    set_digital_pin(map->fire, port->fire);
    set_digital_pin(map->up, port->up);
    set_digital_pin(map->down, port->down);
    set_digital_pin(map->left, port->left);
    set_digital_pin(map->right, port->right);
    set_analog_pin(map->pot_x, port->pot_x);
    set_analog_pin(map->pot_y, port->pot_y);
}

void set_fire_pin(const pin_map_t *pins, bool value) {
    set_digital_pin(pins->fire, value);
}

void init_digital_pin(uint8_t pin) {
#ifdef DIGITAL_OUTPUTS_AS_OPEN_DRAIN
    gpio_init(pin);
    gpio_put(pin, false);
    gpio_disable_pulls(pin);
    gpio_set_dir(pin, GPIO_IN);
#else
    gpio_init(pin);
    set_digital_pin(pin, false);
    gpio_set_dir(pin, GPIO_OUT);
#endif
}

void init_analog_slice(uint8_t pinA, uint8_t pinB) {
    uint8_t sliceA = pwm_gpio_to_slice_num(pinA);
    uint8_t sliceB = pwm_gpio_to_slice_num(pinA);
    if (sliceA != sliceB) {
        LOG_ERROR("PWM slices for pinA and pinB doesn't match.");
        return;
    }
    gpio_set_function(pinA, GPIO_FUNC_PWM);
    gpio_set_function(pinB, GPIO_FUNC_PWM);
    pwm_set_wrap(sliceA, 255);
    pwm_set_chan_level(sliceA, PWM_CHAN_A, 256);
    pwm_set_chan_level(sliceA, PWM_CHAN_B, 256);
    pwm_set_enabled(sliceA, true);
}

void port_init(const pin_map_t *map) {
    init_digital_pin(map->fire);
    init_digital_pin(map->up);
    init_digital_pin(map->down);
    init_digital_pin(map->left);
    init_digital_pin(map->right);
    init_analog_slice(map->pot_x, map->pot_y);
}

void init_uart() {
    uart_init(UART_ID, BAUD_RATE);
    stdio_uart_init();

    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

bool register_device(uint8_t dev_addr, uint8_t interface, device_type_t type) {
    device_t *slot = find_free_device_slot();
    if (slot == NULL) {
        LOG_WARN("No empty device slots.\r\n");
        return false;
    }
    slot->device = dev_addr;
    slot->interface = interface;
    slot->type = type;
    bool ports[NUM_PORTS] = {false};
    for (uint8_t i=0; i<MAX_DEVICES; i++) {
        if (device_map[i].port >= 0 && device_map[i].port < NUM_PORTS) {
            ports[device_map[i].port] = true;
        }
    }
    for (int8_t i=0; i<NUM_PORTS; i++) {
        slot->port = -1;
        if (!ports[i]) {
            slot->port = i;
            break;
        }
    }
    LOG_INFO("Device %u mounted to port %i\r\n", dev_addr, slot->port);
    return true;
}

void remove_device(uint8_t dev_addr) {
    device_t *slot = find_device(dev_addr);
    if (!slot)
        return;
    LOG_INFO("Device %u removed from port %i", dev_addr, slot->port);
    slot->device = 0;
    slot->interface = 0;
    slot->type = NONE;
    slot->port = -1;
}

void write_port_state(device_t *device, c64_control_port_t *state) {
    if (!device || device->port < 0)
        return;
    LOG_TRACE("Output for %i:%i on port %i: ", device->device, device->interface, device->port);
    LOG_TRACE("Fire:     %b", state->fire);
    LOG_TRACE("Up:       %b", state->up);
    LOG_TRACE("Down:     %b", state->down);
    LOG_TRACE("Left:     %b", state->left);
    LOG_TRACE("Right:    %b", state->right);
    LOG_TRACE("Paddle-X: %i", state->pot_x);
    LOG_TRACE("Paddle-Y: %i", state->pot_y);
    const pin_map_t *map = &pin_map[device->port];
    port_write(map, state);
}

void swap_ports(device_t *device) {
#if NUM_PORTS == 1
       return;
#else
    int8_t swap_with = 0;
    if (device->port != NUM_PORTS - 1) {
       swap_with = device->port + 1;
    }
    c64_control_port_t port = {0};
    for(int i=0; i<MAX_DEVICES; i++) {
       if (device_map[i].port == swap_with) {
           write_port_state(&device_map[i], &port);
           device_map[i].port = device->port;
           LOG_INFO("Device %u is now on port %i", device_map[i].device, device_map[i].port);
       }
    }
    write_port_state(device, &port);
    device->port = swap_with;
    LOG_INFO("Device %u is now on port %i", device->device, device->port);
#endif
}
//--------------------------------------------------------------------+
// Blinking Task
//--------------------------------------------------------------------+
void led_blinking_task()
{
    const uint32_t interval_us = 1000 * 1000;
    static uint32_t start_us = 0;

    static bool led_state = false;

    // Blink every interval ms
    uint32_t us = time_us_32();
    if (us - start_us < interval_us) return; // not enough time
    start_us += interval_us;
    gpio_put(LED_PIN, led_state);
    led_state = 1 - led_state; // toggle
}

uint8_t autofire(device_t *pad, uint32_t us) {
    if (pad->autofire_state == 0)
        return 0;
    if (us - pad->last_autofire < AUTOFIRE_INTERVAL * 1000)
        return pad->autofire_state;
    pad->last_autofire = us;
    pad->autofire_state = pad->autofire_state == 1 ? 2 : 1;
    return pad->autofire_state;
}

int main() {
    for (uint8_t i=0; i<MAX_DEVICES; i++) {
        device_t dev = {0, 0, NONE, -1, JOYSTICK, 0, 0};
        device_map[i] = dev;
    }
    init_digital_pin(LED_PIN);
    for (int i=0; i < NUM_PORTS; i++) {
        port_init(&pin_map[i]);
    }
    init_uart();
    tusb_init();

    while (true) {
        sleep_ms(2);
        tuh_task();
        for (uint8_t i=0; i< MAX_DEVICES; i++) {
            uint8_t af = autofire(&device_map[i], time_us_32());
            if (af != 0)
                set_fire_pin(&pin_map[device_map[i].port], af == 1);
        }
        led_blinking_task();
    }

    return 0;
}
