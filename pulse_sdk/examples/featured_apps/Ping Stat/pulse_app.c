/**
 * Ping Stat Example App
 *
 * Description:
 * This app displays a clock and the contents of a pulse protocol
 * notification.  This app comes with a host-side python app
 * that pings a given domain and sends the ping information to
 * the watch once a minute.
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
#include <app_resources.h>

#include <stdint.h>

struct pulse_time_tm current_time;

struct PWTextBox clock_text_box;
struct PWidgetTextDynamic clock_text_widget;

struct PWTextBox message_text_box;
struct PWidgetTextDynamic message_text_widget;

// Text buffers to store text to be rendered
char clock_text_buffer[16];
char message_text_buffer[64];

// Text styles to use for the text widgets
enum PWTextStyle clock_text_style = (PWTS_TRUNCATE | PWTS_CENTER);
enum PWTextStyle message_text_style = (PWTS_WRAP_AT_SPACE | PWTS_CENTER | PWTS_VERTICAL_CENTER);

// Forward Declaration
void handle_message(PulseNotificationId id);

void main_app_init()
{
    // Draw the radar image to the screen
    pulse_dimensions_t d = pulse_get_image_info(IMAGE_RADAR);
    pulse_draw_image(IMAGE_RADAR, (SCREEN_WIDTH - d.width) / 2, 32);

    // Put a default into the message text buffer
    sprintf(message_text_buffer, "Example.com\n??? ms");

    // Set up the text box positions
    clock_text_box.top = 0;
    clock_text_box.bottom = pulse_get_font_height(FONT_CLOCKOPIA_30);
    clock_text_box.left = 0;
    clock_text_box.right = SCREEN_WIDTH - 5;

    message_text_box.top = 105;
    message_text_box.bottom = SCREEN_HEIGHT - 1;
    message_text_box.left = 0;
    message_text_box.right = SCREEN_WIDTH - 1;

    // Initialize the text widgets
    pulse_init_dynamic_text_widget(&clock_text_widget, clock_text_buffer, FONT_CLOCKOPIA_30, COLOR_WHITE24, clock_text_style);
    pulse_init_dynamic_text_widget(&message_text_widget, message_text_buffer, FONT_EASTA_10, COLOR_WHITE24, message_text_style);

    // Register a callback so that we're notified of new pulse protocol notifications
    // (over bluetooth)
    pulse_register_callback(ACTION_NEW_PULSE_PROTOCOL_NOTIFICATION, &handle_message);

    // Write the text widgets to the screen
    update_time();
    update_server_ping_message();
}

// Get the time and write it to a text buffer
struct pulse_time_tm current_time;
void print_time_into_text_buffer()
{    
    pulse_get_time_date(&current_time);
    sprintf(clock_text_buffer, "%d:%02d", current_time.tm_hour, current_time.tm_min);
}

// Print the time to the screen to and set a timer to call
// this function on the minute mark
void update_time()
{
    print_time_into_text_buffer();
    
    // Draw a black box over the text area to clear it
    // There are other more effecient solutions
    pulse_set_draw_window(0,0, SCREEN_WIDTH, 30);
    for (int i = 0; i < 30 * SCREEN_WIDTH; i++) {
        pulse_draw_point24(COLOR_BLACK24);
    }
    pulse_render_text(&clock_text_box, &clock_text_widget);

    // Register a timer to update the time on the minute mark
    pulse_register_timer(60000 - current_time.tm_sec * 1000, &update_time, 0);
}

// Write the message to the screen
void update_server_ping_message()
{
    // Draw a black box over the text area to clear it
    // There are other more effecient solutions
    pulse_set_draw_window(0,102, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (int i = 0; i < 30 * SCREEN_WIDTH; i++) {
        pulse_draw_point24(COLOR_BLACK24);
    }
    pulse_render_text(&message_text_box, &message_text_widget);
}

// This function was registered to receive pulse protocol notifications
// Write the message to the message text buffer
void handle_message(PulseNotificationId id)
{
    struct PulseNotification * p = pulse_get_notification(id);
    sprintf(message_text_buffer, "%s\n%s", p->body, p->sender);
    dbg_printf("%s\n", message_text_buffer);
    update_server_ping_message();
}


void main_app_handle_button_down()
{

}

void main_app_handle_button_up()
{

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

}

void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{

}

