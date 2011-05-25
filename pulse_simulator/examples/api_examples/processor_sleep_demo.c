/**
 * Processor Sleep Demo
 *
 * Description:
 * The processor uses very little power when sleeping.
 * Note that the real time clock (RTC) is still always running,
 * so the clock will still be accurate after waking.
 *
 * If you want the watch battery to last more than 2 - 3 hours,
 * some power management will be necessary.
 *
 * The app puts the processor to sleep 5 seconds after it starts.
 * If the button is pressed to wake the processor, the power down
 * timer is updated for 4 seconds in the future.
 *
 * Before the processor goes in to low power mode (doze), the
 * screen brightness is turned down gradually, creating a fade effect.
 *
 * Note: a bluetooth message will also wake the processor
 *
 *
 * Copyright (C) 2011, Allerta Inc.
 * Author: Jon Bennett (jon@allerta.ca)
 *
 * Permission to use, copy, modify, and/or distribute this software for  
 * any purpose with or without fee is hereby granted, provided that the  
 * above copyright notice and this permission notice appear in all copies.  
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
 * SOFTWARE.
 **/

#include <pulse_os.h>
#include <pulse_types.h>

#include <stdint.h>
#include <stddef.h>

// Forward Declaration
void handle_button_causing_wakeup();

// This function is called once after the watch has booted up
// and the OS has loaded
void main_app_init()
{
    printf("Going to sleep\nin 10 seconds\n");

    // Schedule the processor to go to sleep in 5 seconds
    pulse_update_power_down_timer(10000);

    // Register a callback for the woke_from_button event
    pulse_register_callback(ACTION_WOKE_FROM_BUTTON, &handle_button_causing_wakeup);
}

// This function is called when the button is used to wake the processor
void handle_button_causing_wakeup()
{
    // Set the screen brightness to full
    pulse_oled_set_brightness(100);

    // Schedule the processor to go to sleep in 4 seconds
    pulse_update_power_down_timer(4000);

    printf("Going to sleep\nin 4 seconds\n");
}

void main_app_handle_button_down()
{
    printf("Button Down\n");
}

void main_app_handle_button_up()
{
    printf("Button Up\n");
}

// Main loop. This function is called frequently.
// No blocking calls are allowed in this function or else the watch will reset.
// The inPulse watchdog timer will kick in after 5 seconds if a blocking
// call is made.
void main_app_loop()
{

}

// This function is called whenever the processor is about to sleep (to conserve power)
// The sleep functionality is scheduled with pulse_update_power_down_timer(uint32_t)
void main_app_handle_doz()
{
    // Gradually turn down the screen brightness, creating a fade effect
    for (int i = 100; i >= 0; i-=6) {
        pulse_oled_set_brightness(i);
        pulse_mdelay(60);
    }
}

// Prints a status message to the screen when bluetooth is connected or disconnected
void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{

}
