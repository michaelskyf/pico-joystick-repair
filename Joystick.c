
#include <stdio.h>
#include "device/usbd.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/time.h"
#include "tusb.h"
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 100;
    static uint32_t start_ms = 0;
    static uint8_t button = 0;
    static HID_JoystickReport_Data_t report = {1, 0, 0};

    if ((board_millis() - start_ms) < interval_ms) return; // not enough time
    start_ms = board_millis() + interval_ms;

    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }

    /*------------- Joystick -------------*/
    uint32_t result = adc_read();
    if (tud_hid_ready()) {
        report.xAxis = result;           // 0=left, 128=center, 255=right
        report.yAxis = 128;           // 0=up, 128=center, 255=down

        tud_hid_report(0x00, &report, sizeof(report));
    }
}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t, uint8_t, hid_report_type_t, uint8_t *, uint16_t) {
    // TODO not Implemented

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t, uint8_t, hid_report_type_t, const uint8_t *, uint16_t) {
}

int main()
{
    stdio_init_all(); // inicjacja STDIO
    adc_init();

    adc_gpio_init(26);
    adc_select_input(0);

    board_init();
    tusb_init();

    while(true)
    {
        tud_task();
        hid_task();
    }

    return 0;
}