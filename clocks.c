#include "hardware/clocks.h"

#include <inttypes.h>
#include <stdio.h>

#include "clocks.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "rosc.h"

#include "pico/stdlib.h"

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);

    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);

    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);

    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);

    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);

    uart_default_tx_wait_blocking();
    // Can't measure clk_ref / xosc as it is the ref
}

bool set_peripheral_clk() {
    clock_configure(
        clk_peri, 
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB, 
        48 * 1000,
        48 * 1000
    );

    stdio_init_all();
}

bool set_usb_clk() {
    clock_configure(
        clk_peri, 
        0, 
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB, 
        48 * KHZ,
        48 * KHZ
    );

    stdio_init_all();
}

void run_from_rosc(bool disable_xosc, bool disable_usb) {
    uint32_t src_hz = 6.5 * MHZ;

    // CLK_REF = ROSC
    clock_configure(
        clk_ref, 
        CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 
        0,
        src_hz, 
        src_hz
    );

    // CLK SYS = CLK REF
    clock_configure(
        clk_sys, 
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 
        0, 
        src_hz,
        src_hz
    );

    clock_stop(clk_adc);
    clock_stop(clk_peri);
    pll_deinit(pll_sys);


    if (disable_xosc) {
        xosc_disable();
    }

    if (disable_usb) {
        pll_deinit(pll_usb);
    }

    setup_default_uart();
}

void run_from_xosc(uint32_t requested_freq, bool disable_usb) {
    //requested frequency cannot be greater that source frequency, only divided
    uint32_t src_hz = XOSC_MHZ * MHZ;

    // CLK_REF = XOSC
    clock_configure(
        clk_ref, 
        CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 
        0,
        src_hz, 
        requested_freq
    );

    // CLK SYS = CLK REF
    clock_configure(
        clk_sys, 
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 
        0, 
        requested_freq,
        requested_freq
    );

    clock_stop(clk_adc);
    clock_stop(clk_peri);
    pll_deinit(pll_sys);

    rosc_disable();

    if (disable_usb) {
        clock_stop(clk_usb);
        pll_deinit(pll_usb);
    }

    setup_default_uart();
}

bool reinit_clocks() { clocks_init(); }

void set_sleep_enabled_bits() {
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
    clocks_hw->sleep_en1 = 0x0;
}