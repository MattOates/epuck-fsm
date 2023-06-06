#include "spicomm.h"

/**
 * Exit thread with error message.
 */
void pabort(const char *s)
{
  perror(s);
  abort();
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode = 1;
static uint8_t bits = 16;
static uint32_t speed =20000000;

struct txbuf_t msgTX;
struct rxbuf_t msgRX;

int init_spi()
{
  int fd;
  fd = open(device, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

  /*
  *          * spi mode
  *                   */
  int ret;
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");

  /*
  *          * bits per word
  *                   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word");

  /*
   *          * max speed hz
   *                   */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz");

  return fd;
}

/*send message and read data from dsPIC*/
void do_msg(int fd, int16_t *txbuf, int16_t *rxbuf, int16_t len)
{
  struct spi_ioc_transfer xfer;

  int   status;

  memset(&xfer, 0, sizeof xfer);
  memset(rxbuf, 0, sizeof rxbuf);

  xfer.rx_buf = (__u64) rxbuf;
  xfer.tx_buf = (__u64) txbuf;
  xfer.len = 2*len; //size in bytes

  status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
  if (status < 0)
  {
    perror("SPI_IOC_MESSAGE");
    memset(rxbuf, 0, sizeof rxbuf);
    return;
  }

#ifdef TEST
  uint16_t *bp;
  printf("response(%2d, %2d): ",2* len, status);
  for (bp = rxbuf; len; len--)
    printf(" %d", *bp++);
  printf("\n");
#endif
}

void setSpeed(int16_t lspeed, int16_t rspeed)
{
   msgTX.cmd.set_motor=1;
   msgTX.left_motor = lspeed;
   msgTX.right_motor=rspeed;
}

//get ir sensor data from the data stream
void get_ir_data(int16_t*ir, int16_t size)
{
   int i;
   for(i=0;i<size;i++)
   {
      ir[i]=msgRX.ir[i];
   }
}

//send cmd and get data from dsPIC
void update(int fd)
{
   do_msg(fd,(int16_t *)&msgTX,(int16_t *)&msgRX, sizeof(msgTX)/sizeof(int16_t));
   //may need to reset the cmd array
}

//reset robot
void reset_robot(int fd)
{
    msgTX.cmd.reset = 1;
    update(fd);
}

void setLED(int16_t id, int16_t on)
{
    int16_t temp;
    memcpy(&temp, &(msgTX.led_cmd),2);
    if(on)
      temp |=1<<id;
    else
      temp &= ~(1<<id);
    memcpy(&(msgTX.led_cmd),&temp,2);
}
