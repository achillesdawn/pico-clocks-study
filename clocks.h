void measure_freqs(void);
bool set_peripheral_clk();
bool set_usb_clk();
void run_from_rosc(bool disable_xosc, bool disable_usb);
void run_from_xosc(uint32_t requested_freq, bool disable_usb);
bool reinit_clocks();
#pragma once
