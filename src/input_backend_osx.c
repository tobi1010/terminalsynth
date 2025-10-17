#include "input_backend.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDManager.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

static struct {
    IOHIDManagerRef mgr;
    CFRunLoopRef run_loop;
    CFRunLoopMode loop_mode;
} io_mgr;

static void io_cb(void *context, IOReturn result, void *sender,
                  IOHIDValueRef value) {
    (void)result;
    (void)sender;
    KeyboardState *kb_state = (KeyboardState *)context;
    IOHIDElementRef elem = IOHIDValueGetElement(value);
    uint32_t usage_page = IOHIDElementGetUsagePage(elem);
    uint32_t usage = IOHIDElementGetUsage(elem);
    bool is_pressed = IOHIDValueGetIntegerValue(value) != 0;

    if (usage_page == kHIDPage_KeyboardOrKeypad) {
        if (usage <= 127)
            kb_state->keys[usage] = (is_pressed);
    }
}

void input_backend_init(KeyboardState *state) {
    state->num_keys_pressed = 0;
    io_mgr.mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!io_mgr.mgr) {
        fprintf(stderr, "Failed to create IOHIDManager\n");
        return;
    }

    CFMutableDictionaryRef cf_dict = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);

    int usage_page = kHIDPage_GenericDesktop;
    int usage = kHIDUsage_GD_Keyboard;

    CFNumberRef page_ref =
        CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage_page);
    CFNumberRef usage_ref =
        CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);

    CFDictionarySetValue(cf_dict, CFSTR(kIOHIDDeviceUsagePageKey), page_ref);
    CFDictionarySetValue(cf_dict, CFSTR(kIOHIDDeviceUsageKey), usage_ref);

    CFRelease(page_ref);
    CFRelease(usage_ref);

    IOHIDManagerSetDeviceMatching(io_mgr.mgr, cf_dict);
    CFRelease(cf_dict);

    IOHIDManagerRegisterInputValueCallback(io_mgr.mgr, io_cb, state);

    io_mgr.run_loop = CFRunLoopGetCurrent();
    io_mgr.loop_mode = kCFRunLoopDefaultMode;

    IOHIDManagerScheduleWithRunLoop(io_mgr.mgr, io_mgr.run_loop,
                                    io_mgr.loop_mode);
    IOHIDManagerOpen(io_mgr.mgr, kIOHIDOptionsTypeNone);
}

void input_backend_shutdown(void) {
    IOHIDManagerUnscheduleFromRunLoop(io_mgr.mgr, io_mgr.run_loop,
                                      io_mgr.loop_mode);
    IOReturn ret = IOHIDManagerClose(io_mgr.mgr, kIOHIDOptionsTypeNone);
    if (ret != kIOReturnSuccess) {
        fprintf(stderr, "Failed to close IOHIDManager\n");
        return;
    }
    CFRelease(io_mgr.mgr);
    io_mgr.mgr = NULL;
}

void input_backend_poll(void) { CFRunLoopRunInMode(io_mgr.loop_mode, 0, true); }

static struct termios orig_termios;

void disable_terminal_echo(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON, ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}
void enable_terminal_echo(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}
