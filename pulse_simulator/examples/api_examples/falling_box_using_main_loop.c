/**
 * Falling Box Demo
 * (using main_loop)
 *
 * Description:
 * This app draws a colored box falling down the screen
 * continuosly and is redrawn every 1/2 second.
 * Also, button down and up events are printed to the screen.
 * This example is implemented using the main_app_loop.
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

// This function is called once after the watch has booted up
// and the OS has loaded
void main_app_init()
{

}

void main_app_handle_button_down()
{
    printf("Button Down\n");
}

void main_app_handle_button_up()
{
    printf("Button Up\n");
}

void draw_box(int top_left_coord, color24_t color)
{
    // Set the canvas dimensions to be a 20 x 20 box
    pulse_set_draw_window(0, top_left_coord, 20 - 1, top_left_coord + 20 - 1);

    // Draw pixels onto that canvas
    for (int i = 0; i < 20 * 20; i++) {
        pulse_draw_point24(color);
    }
}

uint32_t time_last_box_drawn_ms = 0;
int y_coord_top = 0;

// Main loop. This function is called frequently.
// No blocking calls are allowed in this function or else the watch will reset.
// The inPulse watchdog timer will kick in after 5 seconds if a blocking
// call is made.
void main_app_loop()
{
    // Find out how long the processor has been active (since boot or since sleep)
    // Since we are not putting the processor to sleep in this example app at all,
    // these are the number of ms since boot.
    uint32_t time_now_ms = pulse_get_millis();

    // Check if it's been at least 500 ms since the box was last drawn
    if (time_now_ms - time_last_box_drawn_ms >= 500) {
        
        // Random color.  Looks sort of green.
        color24_t color = { 0x44, 0x99, 0x11, 0x00 };

        // Draw a black box on top of the old one to "hide" it
        draw_box(y_coord_top, COLOR_BLACK24);

        // Incrememnt the y coordinate and wrap around if we're hitting the bottom of the screen
        y_coord_top = (y_coord_top + 5) % (SCREEN_HEIGHT - 20 - 1);

        // Draw the new box and update the time_last_box_draw_ms variable
        draw_box(y_coord_top, color);
        time_last_box_drawn_ms = time_now_ms;
    }
    
}

// This function is called whenever the processor is about to sleep (to conserve power)
// The sleep functionality is scheduled with pulse_update_power_down_timer(uint32_t)
void main_app_handle_doz()
{

}

void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{

}
