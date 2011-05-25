/**
 * Bluetooth Data Demo
 *
 * Description:
 * This app prints the first 16 bytes of a bluetooth data buffer to
 * the screen and send back a short response.
 *
 * Note:
 * You'll need to send bluetooth data to the watch and be able to 
 * receive data back to see anything interesting.
 *
 * Note that the current max size for a bluetooth data buffer is 255 bytes
 * for both sending and receiving.
 * (as of January 2011)
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
void handle_received_bluetooth_data(const uint8_t *buffer);

// This function is called once after the watch has booted up
// and the OS has loaded
void main_app_init()
{
    printf("BT Data Printer\n\nWaiting for\ndata ...\n");

    // Registers a callback function to handle non pulse-protocol bluetooth data received
    pulse_register_callback(ACTION_HANDLE_NON_PULSE_PROTOCOL_BLUETOOTH_DATA, &handle_received_bluetooth_data);
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

// Send back a short response to the host that initiated the bluetooth connection
void send_bluetooth_response()
{
    uint32_t response = 1;
    pulse_send_bluetooth_int(response);
}

// Prints the first 16 bytes of the bluetooth data received to the screen
void handle_received_bluetooth_data(const uint8_t *buffer)
{
    for (int i = 0; i < 16; i++) {
        printf("0x%x,", buffer[i]);
    }
}
