/**
 * Pulse Protocol Notification Demo
 *
 * Description:
 * Prints out pulse protocol notification messages received.
 * On a button press, it prints up to the last 8 PulseNotificationIds
 * for the last 8 received pulse protocol notifications.
 *
 * The pulse protocol packet format is described in /doc/pulse_protocol.odt.
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
void handle_new_pulse_protocol_notification(PulseNotificationId id);

// This function is called once after the watch has booted up
// and the OS has loaded
void main_app_init()
{
    printf("Pulse Protocol\nNotification\nReceiver\n\n");

    // Registers a callback function to handle new pulse protocol notifications
    pulse_register_callback(ACTION_NEW_PULSE_PROTOCOL_NOTIFICATION, &handle_new_pulse_protocol_notification);
}

void main_app_handle_button_down()
{
    // Create an array to hold the PulseNotificationIds
    PulseNotificationId latest_notifications[MAX_NOTIFICATIONS_PER_LIST];

    // Call a function to populate the array
    pulse_get_notification_list_by_type(PNT_ALL, latest_notifications);

    // Clear the screen
    pulse_blank_canvas();

    // Print the latest notification ids to the screen
    printf("Latest\nNotification IDs\n\n");
    for (int i = 0; i < MAX_NOTIFICATIONS_PER_LIST; i++) {

        // If less than 8 notifications have been received, the array will
        // contains NULLs
        if (latest_notifications[i] == NULL) {
            break;
        }
        
        printf("0x%x\n", latest_notifications[i]);
    }
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

// Prints a status message to the screen when bluetooth is connected or disconnected
void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{
    switch(event) {
        case BLUETOOTH_CONNECTED:
            printf("BT Connected\n");
            break;

        case BLUETOOTH_DISCONNECTED:
            printf("BT Disconnected\n");
            break;
    }
}

// Gets the notification based on the id and prints the messages to the screen
void handle_new_pulse_protocol_notification(PulseNotificationId id)
{
    pulse_blank_canvas();
    struct PulseNotification *notification = pulse_get_notification(id);
    printf("Sender:\n%s\n\n", notification->sender);
    printf("Body:\n%s\n", notification->body);
}
