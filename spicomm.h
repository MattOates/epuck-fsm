#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#ifndef _SPICOMM_H
#define _SPICOMM_H

struct cmd_t{
	int16_t set_motor:1;
	int16_t set_led:1;
	int16_t reset:1;
    int16_t cal_ir:1;
	int16_t cal_acc:1;
	int16_t read_ir:1;
	int16_t read_light:1;
	int16_t read_mic:1;
	int16_t read_acc:1;
	int16_t reseverd:7;
};

struct led_cmd_t{
	int16_t led0:1;
	int16_t led1:1;
	int16_t led2:1;
	int16_t led3:1;
	int16_t led4:1;
	int16_t led5:1;
	int16_t led6:1;
	int16_t led7:1;
	int16_t bodyled:1;
	int16_t frontled:1;
	int16_t reserved:6;
};

struct txbuf_t
{
    struct cmd_t cmd;		//first two bytes for commands
    int16_t left_motor;	//speed of left motor
    int16_t right_motor;	//speed of right motor
    struct led_cmd_t led_cmd;	//command for leds
    int16_t led_cycle;		// blinking rate of LEDS
    int16_t reserved[19];	//reserved, in order to makde the txbuf_t and rxbuf_t are in the same size, now 25 16bits words
    int16_t dummy;		// leave it empty
};
struct rxbuf_t
{
    int16_t ir[8];			// IR Ranges
    int16_t acc[3];			// Accelerometer x/y/z
    int16_t mic[3];			// microphones 1,2,3
    int16_t amb[8];			// Ambient IR
    int16_t tacl;			// steps made on l/r motors
    int16_t tacr;
    int16_t batt;			// battery level
};

int init_spi();
void do_msg(int fd, int16_t *txbuf, int16_t *rxbuf, int16_t len);
void setSpeed(int16_t lspeed, int16_t rspeed);
void get_ir_data(int16_t *ir, int16_t size);
void setLED(int16_t id, int16_t on);
void reset_robot(int fd);

#endif
