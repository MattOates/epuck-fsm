#include "support.h"
#define KEY_BOARD_PATH "/dev/input/event0"
#define SWITCHES_PATH  "/dev/epuck_switch"

int get_switch()
{
  int fd = open(SWITCHES_PATH, O_RDONLY);
  char buf[4];
  if (fd < 0)
  {
    perror("open epuck_switch");
    return 99;
  }
  int ret = read(fd, buf, sizeof(buf));
  if (ret < 4)
  {
    perror("read switch");
    return 99;
  }
  int value;
  sscanf(buf, "%2d\n", &value);
  close(fd);
  return value;
}

int set_board_led(int led_no, int on)
{
  char str[256];
  int fd;

  if (led_no < 0 || led_no > 3)
  {
    printf("only support 'user1' and 'user2' leds now!\n");
    return 0;
  }

  //set brightness
  sprintf(str, "/sys/class/leds/user%d/brightness", led_no);
  if ((fd = open(str, O_WRONLY)) < 0)
  {
    perror("open");
    return 0;
  }
  sprintf(str, "%d\n", 255 * on);
  if (write(fd, str, strlen(str)) != strlen(str))
  {
    perror("set brightness");
    close(fd);
    return 0;
  }
  close(fd);
  return 1;
}

int blink_board_led(int led_no, int delay_on, int delay_off)
{
  char str[256];
  int fd;

  if (led_no < 0 || led_no > 3)
  {
    printf("only support 'user1' and 'user2' leds now!\n");
    return 0;
  }

  //set delay_on
  sprintf(str, "/sys/class/leds/user%d/delay_on", led_no);
  if ((fd = open(str, O_WRONLY)) < 0)
  {
    perror("open");
    return 0;
  }
  sprintf(str, "%d\n", delay_on);
  if (write(fd, str, strlen(str)) != strlen(str))
  {
    perror("set delay_on");
    close(fd);
    return 0;
  }
  close(fd);

  //set delay_off
  sprintf(str, "/sys/class/leds/user%d/delay_off", led_no);
  if ((fd = open(str, O_WRONLY)) < 0)
  {
    perror("open");
    return 0;
  }
  sprintf(str, "%d\n", delay_off);
  if (write(fd, str, strlen(str)) != strlen(str))
  {
    perror("set brightness");
    close(fd);
    return 0;
  }
  close(fd);

  return 1;


}

//this function will block until the specified button are pressed
int wait_btn_trigger(unsigned short code)
{
  int fd = -1;
  if ((fd = open(KEY_BOARD_PATH, O_RDONLY)) < 0)
  {
    perror("evdev open");
    return -1;
  }

  /* how many bytes were read */
  size_t rb;
  int yalv;
  /* the events (up to 64 at once) */
  struct input_event ev[64];
  while (1)
  {

    rb = read(fd, ev, sizeof(struct input_event) * 64);

    if (rb < (int) sizeof(struct input_event))
    {
      perror("evtest: short read");
      return -1;
    }

    for (yalv = 0; yalv < (int) (rb / sizeof(struct input_event));
         yalv++)
    {
      if (EV_KEY == ev[yalv].type && code == ev[yalv].code)
      {
        printf("%ld.%06ld ", ev[yalv].time.tv_sec,  ev[yalv].time.tv_usec);
        printf("type %d code %d value %d\n", ev[yalv].type, ev[yalv].code, ev[yalv].value);
        close(fd);
        return 1;
      }
    }
  }

}
