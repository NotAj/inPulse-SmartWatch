/*
 * Copyright (C) 2009 by Matthias Ringwald
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MATTHIAS RINGWALD AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  test code to talk with inPulse watch
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <btstack/btstack.h>
#include <btstack/hci_cmds.h>

/* Definitions for endpoint numbers */
#define PP_ENDPOINT_CONTROL 0
#define PP_ENDPOINT_NOTIFICATION 1
#define PP_ENDPOINT_COMMAND 2
#define PP_ENDPOINT_SPIFLASH 3


// address of watch
bd_addr_t addr = {0x00, 0x50, 0xC2, 0x79, 0xED, 0x38};	// inPulse


typedef struct {
	uint8_t header_length;
	uint8_t length;
	uint8_t endpoint;
	uint8_t sequence;
	uint32_t time;
} __attribute__((packed)) pp_message_t;

enum commands
{
	command_query_vitals			   = 0,
	command_read_setting			   = 1,
	command_write_setting		       = 2,
	command_erase_spiflash_sector	   = 32,
	command_enable_unsafe_spiflash_ops = 33,
	command_erase_element			   = 40,
	command_set_time			       = 50,
	command_reboot_watch		       = 255,
};

typedef struct {
	pp_message_t message_header;
	uint16_t command;
	uint32_t parameter1;
	uint32_t parameter2;
} __attribute__((packed)) pp_command_t;

typedef struct {
	pp_message_t message_header;
	uint8_t	notification_type;  // The type of notification (e-mail/sms/ calender/phone).
	uint8_t	alert_field;		// Bitmask of hardware alerts to activate (flash/double flash/vibrate/double vibrate)
	uint8_t payload[0];
} __attribute__((packed)) pp_notify_t;

int set_time(int source_cid)
{
	struct timecmd {
		pp_command_t cmd;
		struct tm ts;
	} __attribute__((packed)) timecmd;

	time_t now;
	struct tm *ts;

	now = time(NULL);
	ts = localtime(&now);
	memcpy(&timecmd.ts, ts, sizeof(struct tm));

	timecmd.cmd.message_header.endpoint = PP_ENDPOINT_COMMAND;
	timecmd.cmd.message_header.header_length = sizeof(timecmd.cmd.message_header);
	timecmd.cmd.message_header.length = sizeof(timecmd);
	timecmd.cmd.message_header.sequence = 0;
	timecmd.cmd.command = command_set_time;
	timecmd.cmd.parameter1 = now;
	timecmd.cmd.parameter2 = +1;
	
    bt_send_l2cap( source_cid, (uint8_t*) &timecmd, 50);
}

int store_inpulse_string(char *dest, const char *string){
	int len = strlen(string) + 1;
	*dest++ = len;
	strcpy(dest, string);
	return len+1;
}

int test_sms_notify(int source_cid)
{
	uint8_t buffer[256];
	pp_notify_t * message = (pp_notify_t*) &buffer;
	message->message_header.endpoint = PP_ENDPOINT_NOTIFICATION;
	message->message_header.header_length = sizeof(message->message_header);
	message->message_header.sequence = 0;
	message->notification_type = 1; //sms
	message->alert_field = 4;	// vibrate
	int pos = sizeof(pp_notify_t);
	int len;
	len = store_inpulse_string((char*)&buffer[pos], "SMS Sender"); pos += len;
	len = store_inpulse_string((char*)&buffer[pos], "SMS Hey there! Wanna go for a beer after your ride?");
	pos += len;
	message->message_header.length = pos;
	
    bt_send_l2cap( source_cid, (uint8_t*) &buffer, pos);
}


hci_con_handle_t con_handle;
uint16_t source_cid_interrupt;
uint16_t source_cid_control;

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
	
	bd_addr_t event_addr;

	switch (packet_type) {
			
		case L2CAP_DATA_PACKET:
			// just dump data for now
			printf("source cid %x -- ", channel);
			hexdump( packet, size );
	
			// HOME => disconnect
			if (packet[0] == 0xA1) {// Status report
				if (packet[1] == 0x30 || packet[1] == 0x31) {   // type 0x30 or 0x31
					if (packet[3] & 0x080) {  // homne button pressed
						printf("Disconnect baseband\n");
						bt_send_cmd(&hci_disconnect, con_handle, 0x13); // remote closed connection
					}
				}
			}
			break;
			
		case HCI_EVENT_PACKET:
			
			switch (packet[0]) {

				case BTSTACK_EVENT_POWERON_FAILED:
					printf("HCI Init failed - make sure you have turned off Bluetooth in the System Settings\n");
					exit(1);
					break;
					
				case BTSTACK_EVENT_STATE:
					// bt stack activated, get started - disable pairing
					if (packet[2] == HCI_STATE_WORKING) {
						bt_send_cmd(&hci_write_authentication_enable, 0);
					}
					break;
					
				case HCI_EVENT_LINK_KEY_REQUEST:
					printf("HCI_EVENT_LINK_KEY_REQUEST \n");
					// link key request
					bt_flip_addr(event_addr, &packet[2]); 
					bt_send_cmd(&hci_link_key_request_negative_reply, &event_addr);
					break;
					
				case HCI_EVENT_PIN_CODE_REQUEST:
					// inform about pin code request
					printf("Please enter PIN 4846 on remote device\n");
					bt_flip_addr(event_addr, &packet[2]); 
					bt_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, "4846");
					break;
					
				case L2CAP_EVENT_CHANNEL_OPENED:
					// inform about new l2cap connection
					// inform about new l2cap connection
					bt_flip_addr(event_addr, &packet[3]);
					uint16_t psm = READ_BT_16(packet, 11); 
					uint16_t source_cid = READ_BT_16(packet, 13); 
					con_handle = READ_BT_16(packet, 9);
					if (packet[2] == 0) {
						printf("Channel successfully opened: ");
						print_bd_addr(event_addr);
						printf(", handle 0x%02x, psm 0x%02x, source cid 0x%02x, dest cid 0x%02x\n",
							   con_handle, psm, source_cid,  READ_BT_16(packet, 15));
						
						// connected, let's do something
						
						// 1.
						test_sms_notify(source_cid);
						
						// 2.
						set_time(source_cid);
                        
					} else {
						printf("L2CAP connection to device ");
						print_bd_addr(event_addr);
						printf(" failed. status code %u\n", packet[2]);
						exit(1);
					}
					break;
					
				case HCI_EVENT_DISCONNECTION_COMPLETE:
					// connection closed -> quit tes app
					printf("Basebank connection closed, exit.\n");
					exit(0);
					break;
					
				case HCI_EVENT_COMMAND_COMPLETE:
					// connect to HID device (PSM 0x13) at addr
					if ( COMMAND_COMPLETE_EVENT(packet, hci_write_authentication_enable) ) {
						printf("Connecting to inPulse\n");
						bt_send_cmd(&l2cap_create_channel, addr, 0x1001);
					}
					break;
					
				default:
					// other event
					break;
			}
			break;
			
		default:
			// other packet type
			break;
	}
}

int main (int argc, const char * argv[]){
	run_loop_init(RUN_LOOP_POSIX);
	int err = bt_open();
	if (err) {
		printf("Failed to open connection to BTdaemon\n");
		return err;
	}
	bt_register_packet_handler(packet_handler);
	bt_send_cmd(&btstack_set_power_mode, HCI_POWER_ON );
	run_loop_execute();
	bt_close();
}
