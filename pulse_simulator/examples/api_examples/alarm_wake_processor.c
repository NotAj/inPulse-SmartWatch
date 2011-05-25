/**
 * Alarm Wake Processor (Clock Chime) Demo
 *
 * Description:
 * This app prints the time to the screen and then puts the processor
 * to sleep.  On every 15 minute marker (12:00, 12:15, etc.),
 * an alarm is fired and the processor wakes up.
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

#define POWER_DOWN_TIMEOUT_MS (10000)

// Global variable for time
struct pulse_time_tm current_time;

// Forward declarations
void handle_button_causing_wakeup();
void handle_alarm_firing();
void set_alarm();

void main_app_init() {
    // Register OS callbacks
    pulse_register_callback(ACTION_WOKE_FROM_BUTTON, &handle_button_causing_wakeup);
    pulse_register_callback(ACTION_ALARM_FIRING, &handle_alarm_firing);

    set_alarm();

    pulse_update_power_down_timer(POWER_DOWN_TIMEOUT_MS);
}

void print_time_to_screen()
{
    // Get the current time and print it to the screen
    pulse_get_time_date(&current_time);

    pulse_blank_canvas();
    printf("%02d:%02d\n", current_time.tm_hour, current_time.tm_min);
}

void set_alarm() {
    // Print time to the screen
    print_time_to_screen();
    
    PulseAlarm alarm;
    
    // Make sure it's active today
    alarm.daysActive[current_time.tm_wday] = true;

    // Set up vibe pattern
    // Patern is set for one long, followed by 3 short vibes
    alarm.alert.type = 1;     // Type 1 = vibrate (set to 0 for no vibe)
    alarm.enabled = true;
    
    alarm.alert.on1 =  75;    // 75 x 10 ms = 750 ms ON
    alarm.alert.off1 = 50;    // 50 x 10 ms = 500 ms OFF
    alarm.alert.on2 =  20;    // 20 x 10 ms = 200 ms ON
    alarm.alert.off2 = 20;    // etc ...
    alarm.alert.on3 =  20;
    alarm.alert.off3 = 20;
    alarm.alert.on4 =  20;

    // Set the alarm hour (if min > 45, need to set the alarm hour ahead by 1)
    // Correct for minutes possibly wrapping around with % 60
    alarm.hour = (current_time.tm_hour + ((current_time.tm_min > 45) ? 1 : 0)) % 24;

    // Set the alarm minute to be the next 15 minute marker (0, 15, 30, 45)
    // Correct for minutes possibly wrapping around with % 60
    alarm.min = (((current_time.tm_min / 15) + 1) * 15) % 60;

    // Note: setting this alarm perists across watch resets
    pulse_set_alarm(&alarm);

    // Get the active alarm and confirm that it's enabled
    if (pulse_get_alarm()->enabled != true) {
        // Shouldn't ever hit this
        printf ("Alarm not enabled!\n\nOh Noes!\n");
    }

    printf("\n\nAlarm Set:\n%02d:%02d\n", alarm.hour, alarm.min);
}

void handle_button_causing_wakeup() {
    // Alarm doesn't really need to be set again, but that will print
    // the time to the screen and set the power down timer as well
    set_alarm();
}

// This function is called when the alarm is fired
// This function was registered in main_app_init()
void handle_alarm_firing()
{
    set_alarm();
    pulse_update_power_down_timer(POWER_DOWN_TIMEOUT_MS);
}

void main_app_handle_button_down() {
}

void main_app_handle_button_up() {
}

void main_app_loop() {
}

void main_app_handle_doz() {
}

void main_app_handle_hardware_update(enum PulseHardwareEvent event) {
}

