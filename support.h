#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>

#ifndef SUPPORT_H
#define SUPPORT_H

int get_switch();
int set_board_led(int led_no,int on);
int blink_board_led(int led_no,int delay_on, int delay_off);
int wait_btn_trigger(unsigned short code);
#endif
