
#ifndef PICOC64USB_CONFIG_H
#define PICOC64USB_CONFIG_H

//#define DIGITAL_OUTPUTS_AS_OPEN_DRAIN

#define INVERT_DIGITAL_OUTPUTS
// Joystick settings
#define JOYSTICK_AUTOFIRE TRIGGER_TO_DIGITAL(TRIGGER_RIGHT)
#define JOYSTICK_FIRE     BUTTON_B
#define JOYSTICK_UP       OR(DPAD_UP, STICK_AXIS_TO_DIGITAL(STICK_LEFT_Y, DIRECTION_UP))
#define JOYSTICK_DOWN     OR(DPAD_DOWN, STICK_AXIS_TO_DIGITAL(STICK_LEFT_Y, DIRECTION_DOWN))
#define JOYSTICK_LEFT     OR(DPAD_LEFT, STICK_AXIS_TO_DIGITAL(STICK_LEFT_X, DIRECTION_LEFT))
#define JOYSTICK_RIGHT    OR(DPAD_RIGHT, STICK_AXIS_TO_DIGITAL(STICK_LEFT_X, DIRECTION_RIGHT))
#define JOYSTICK_SWAP_PORTS BUTTON_SYNC

// Optional analog inputs in Joystick mode
#define JOYSTICK_PADDLE_X          NOT_CONNECTED
#define JOYSTICK_PADDLE_Y          NOT_CONNECTED

// Mouse Settings
#define MOUSE_ANALOG_X    STICK_LEFT_X
#define MOUSE_ANALOG_Y    STICK_LEFT_Y
// Digital Inputs
#define MOUSE_BUTTON_RIGHT BUTTON_A
#define MOUSE_BUTTON_LEFT  BUTTON_B

// in ms
#define AUTOFIRE_INTERVAL    100

// Analog stick deadzone
#define STICK_DEADZONE       2000
#define STICK_LEFT_DEADZONE  STICK_DEADZONE
#define STICK_RIGHT_DEADZONE STICK_DEADZONE

// For analog to digital conversion
// as a value between 0 and 1
#define DIGITAL_TRIGGER_THRESHOLD     0.75
#define DIGITAL_AXIS_THRESHOLD        0.6


// Don't change anything below this point unless you know what you are doing
// Max values are the positive maximum of an axis
#define TRIGGER_MAX_VALUE              0x03ff
// Axis value can be -0x8000 .. 0x7fff
#define AXIS_MAX_VALUE                 0x7fff

#define NOT_CONNECTED    0
// Digital Inputs
#define BUMPER_LEFT      CFG_VAR(bumper_left)
#define BUMPER_RIGHT     CFG_VAR(bumper_right)
#define BUTTON_A         CFG_VAR(a)
#define BUTTON_B         CFG_VAR(b)
#define BUTTON_X         CFG_VAR(x)
#define BUTTON_Y         CFG_VAR(y)
#define DPAD_UP          CFG_VAR(dpad_up)
#define DPAD_DOWN        CFG_VAR(dpad_down)
#define DPAD_LEFT        CFG_VAR(dpad_left)
#define DPAD_RIGHT       CFG_VAR(dpad_right)
#define BUTTON_SYNC      CFG_VAR(sync)
#define BUTTON_START     CFG_VAR(start)
#define BUTTON_BACK      CFG_VAR(back)
#define STICK_LEFT_CLICK CFG_VAR(stick_left_click)
#define STICK_RIGHT_CLICK CFG_VAR(stick_right_click)

// Analog Inputs
#define STICK_LEFT_X     CFG_VAR(stick_left_x)
#define STICK_LEFT_Y     CFG_VAR(stick_left_y)
#define STICK_RIGHT_X    CFG_VAR(stick_right_x)
#define STICK_RIGHT_Y    CFG_VAR(stick_right_y)
#define TRIGGER_LEFT     CFG_VAR(trigger_left)
#define TRIGGER_RIGHT    CFG_VAR(trigger_right)

// Hacky way of creating a configuration language...
#define INPUT_VAR        input
#define CFG_VAR2(v,i)    v->i
#define CFG_VAR(a)       CFG_VAR2(INPUT_VAR,a)
#define OR(a, b)         (a) || (b)
#define OR3(a, b, c)     (a) || (b) || (c)
#define OR4(a, b, c, d)  (a) || (b) || (c) || (d)
#define AND(a, b)        (a) && (b)
#define AND3(a, b, c)    (a) && (b) && (c)
#define AND4(a, b, c, d) (a) && (b) && (c) && (d)

#define TRIGGER_TO_DIGITAL(a)         trigger_to_digital(a)
#define STICK_AXIS_TO_DIGITAL(a, d)   stick_axis_to_digital((a) * (d))
#define DIRECTION_LEFT                (-1)
#define DIRECTION_RIGHT               1
#define DIRECTION_UP                  1
#define DIRECTION_DOWN                (-1)
#define THRESHOLD_FORMULA(t, m)       ((t) * (m))

#define TRIGGER_DIGITAL_THRESHOLD     THRESHOLD_FORMULA(DIGITAL_TRIGGER_THRESHOLD, TRIGGER_MAX_VALUE)
#define AXIS_DIGITAL_THRESHOLD        THRESHOLD_FORMULA(DIGITAL_AXIS_THRESHOLD, AXIS_MAX_VALUE)

#endif //PICOC64USB_CONFIG_H
