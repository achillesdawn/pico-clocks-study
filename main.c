#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/clocks.h"
#include "pico/stdlib.h"

#include "clocks.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

const uint RED_LED = 16;
const uint YELLOW_LED = 17;
const uint GREEN_LED = 18;
const uint BLUE_LED = 19;

const uint PIN_MASK =
    (1 << RED_LED) | (1 << YELLOW_LED) | (1 << GREEN_LED) | (1 << BLUE_LED);

int main() {
    stdio_init_all();
    gpio_init_mask(PIN_MASK);
    gpio_set_dir_out_masked(PIN_MASK);

    // set_sys_clock_48mhz();

    sleep_ms(2000);
    measure_freqs();
    datetime_t date_time = {
        .year = 2024,
        .month = 07,
        .day = 26,
        .dotw = 5, // 0 is Sunday, so 5 is Friday
        .hour = 00,
        .min = 00,
        .sec = 00
    };

    rtc_init();
    rtc_set_datetime(&date_time);

    printf("datetime: %d %d %d", date_time.hour, date_time.min, date_time.sec);

    puts("running test");

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t led = RED_LED; led <= BLUE_LED; led++) {
            gpio_put(led, true);
            sleep_ms(1000);
            gpio_put(led, false);
            sleep_ms(1000);
        }
    }

    rtc_get_datetime(&date_time);

    printf("datetime: %d %d %d", date_time.hour, date_time.min, date_time.sec);

    puts("about to set clocks");

    // run_from_rosc(true, true);

    // gpio_init(16);
    // gpio_set_dir(16, GPIO_OUT);

    // uint8_t count = 0;
    // bool toggle = false;
    // while (true) {
    //     gpio_put(16, toggle);
    //     toggle = !toggle;
    //     printf("sleeping %d\n", count);
    //     sleep_ms(1000);
    //     count += 1;

    //     if (count % 5 == 0) {
    //         measure_freqs();
    //         break;
    //     }
    // }

    // xosc runs at 12 MHZ in source(see function documentation)
    // setting the xosc at 6 MHZ affects functions like sleep.
    // at lower frequencies, sleep will take longer because the system is slower
    // compensate by sleeping less by the factor of X / 12MHZ. e.g. 6 MHZ / 12
    // MHZ = 0.5
    run_from_xosc(6 * MHZ, false);
    puts("running from xosc");

    measure_freqs();

    uint8_t count = 0;
    uint8_t freq = 1;

    while (true) {

        for (uint8_t led = RED_LED; led <= BLUE_LED; led++) {
            gpio_put(led, true);
            printf("sleeping %d\n", count);
            sleep_ms(500);

            // using the realtime clock to make sure we are sleeping
            // should print 1 second difference between sleeps
            rtc_get_datetime(&date_time);
            printf(
                "datetime: %d %d %d",
                date_time.hour,
                date_time.min,
                date_time.sec
            );

            gpio_put(led, false);
            sleep_ms(500);

            // using the realtime clock to make sure we are sleeping
            // should print 1 second difference between sleeps
            rtc_get_datetime(&date_time);
            printf(
                "datetime: %d %d %d",
                date_time.hour,
                date_time.min,
                date_time.sec
            );
        }

        count += 1;

        if (count % 5 == 0) {
            measure_freqs();
        }
    }
    return 0;
}
