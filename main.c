#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#include "spicomm.h"
#include "support.h"

#define ON 1
#define OFF 0

/*testing*/
#define EPUCK 1

typedef struct motion_t
{
	int speed;
	int turnrate;
	int duration;
};
#define MAX_MOTION 10
struct motion_t motionList[MAX_MOTION];
int header=0;
int tail=0;

int userQuit = 0;

void signalHandler(int dummy);
void stopRobot(void *ptr);
void cmdShell(void *ptr);
void timerHandler(int dummy);
int fd;

void setSpeed2(int speed, int turnrate);
int parseCMD(char * buf);
void printMotionList(struct motion_t * list);

//format [speed, turn_rate, duration]
bool newCommand = true; //flag
bool robotStoped=1;
int currentTime=0;
int lastupdateTime=0;

#define BUFFSIZE 80
char command[BUFFSIZE];



int main(int argc, char*argv[])
{
    
    //set signal handler to capture "ctrl+c" event
    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        printf("signal(2) failed while setting up for SIGINT");
        return -1;
    }

    if (signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        printf("signal(2) failed while setting up for SIGTERM");
        return -1;
    }

    //set signal handler to capture timer out event
    if (signal(SIGALRM, timerHandler) == SIG_ERR)
    {
        printf("signal(2) failed while setting up for SIGALRM");
        return -1;
    }

#ifdef EPUCK
    fd = init_spi();
    if (fd == -1)
    {
        pabort("fail to initialise SPI interface, exit");
        return -1;
    }
    //turn on the light showing it is connected
    set_board_led(1, ON);
#endif

    //creat thread  monitering command line
    pthread_t thread;
    pthread_create(&thread, NULL, cmdShell, NULL);
    
    usleep(1000000);
    


    //set timer to be every 100 ms
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 100000;
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 100000;

    //set timer
    if (setitimer(ITIMER_REAL, &tick, NULL))
    {
        printf("Set timer failed!!\n");
    }





    int leftwheel = 0;
    int rightwheel = 0;

    int lastcmdIndex=0;

    while (1)
    {

        userQuit = 0;
        //main loop
        while (userQuit != 1)
        {

	    if(newCommand)
	    {
		lastcmdIndex = header-1;
		if(lastcmdIndex<0)
		   lastcmdIndex = MAX_MOTION-1;
		    printf("\n----%d set new speed: %d\tturn rate:%d\tduration:%d\nepuck>",lastcmdIndex,motionList[header].speed, motionList[header].turnrate,motionList[header].duration);
		    fflush(stdout);
		    newCommand=false;
		    setSpeed2(motionList[header].speed,motionList[header].turnrate);
		    motionList[lastcmdIndex].speed=0;
		    motionList[lastcmdIndex].turnrate=0;
		    motionList[lastcmdIndex].duration=0;
	    }

	    usleep(50000);
        }
		
		if(!robotStoped)
		{
			robotStoped = true;
			printf("\nstop the robot\nepuck>");
			fflush(stdout);
			setSpeed2(0,0);
			motionList[header].speed=0;
		    motionList[header].turnrate=0;
		    motionList[header].duration=0;
		}
	
    }

    //turn off board led showing program is ended
    set_board_led(1, OFF);
    setSpeed(0, 0);
    update(fd);

    return 0;
}

void timerHandler(int dummy)
{
    int lastmotionIndex = header-1;
    if(lastmotionIndex<0)
    	lastmotionIndex=MAX_MOTION-1;
    currentTime++;
    if(header==tail && currentTime-lastupdateTime>=motionList[header].duration)
    {
	    lastupdateTime=currentTime;
	    userQuit=1;
	    newCommand=false;
    }
    else if(currentTime-lastupdateTime>=motionList[header].duration)
    {
	    lastupdateTime=currentTime;
	    newCommand=true;
	    robotStoped=false;
	    header++;
	    if(header==MAX_MOTION)
		    header=0;
    }
}


#ifdef EPUCK
void signalHandler(int dummy)
{
    printf("Ctrl+C captured, exit program!\n");
    set_board_led(1, OFF);
    setSpeed(0, 0);
    update(fd);
    userQuit = 1;
    exit(0);
}
#else
void signalHandler(int dummy)
{
    printf("Ctrol+C captured, exit program!\n");
    userQuit=1;
    exit(0);
}
#endif

#ifdef EPUCK
void stopRobot(void* ptr)
{
    printf("pthread is running\n");
    while (1)
    {
        wait_btn_trigger(0x101);
        userQuit = 1;
    }
}
#else
void stopRobot(void* ptr)
{
}
#endif

void cmdShell(void *ptr)
{
	
        printf("\n-----------------------------------------------\n");
        printf("Simple command shell to control the epuck robot\n");
        printf("Available command:\n");
        printf("\tmove [speed] [duration]\n");
        printf("\tturn [speed] [duration]\n");
        printf("-----------------------------------------------\n\n");


	while(1)
	{
		printf("epuck>");
		if(fgets(command, sizeof(command),stdin)==NULL)
		{
			printf("fault! exit\n");
			exit(0) ;
		}
		else if(!parseCMD(command))
		{
			userQuit=1;
			exit(0);
		}
	}

}

int parseCMD(char * buf)
{
	char cmd[16];
	int arg1=0,arg2=0, arg3;
	int res=0;
	int i=0;
	if((res=sscanf(buf, "%s %d %d %d\n", cmd, &arg1, &arg2 , &arg3))!=EOF)
	{
		switch(res)
		{
			case 1:
			  if(strcmp(cmd,"exit")==0)
			  {
				  printf("exit epuck command shell\n");
				  return 0;
			  } 
			  else if(strcmp(cmd,"reset")==0)
			  {
				  printf("clear motion list\n");
				  header=tail=0;
			  }
			  else if(strcmp(cmd,"help")==0)
			  {
			  	  printf("available command\n");
				  printf("reset:\t clear motion list\n");
				  printf("exit:\t exit epuck command sheel\n");
				  printf("move [speed][duration]:\tmove at [speed] for [duration] ms\n");
				  printf("turn [turnrate][duration]:\tturn at [turnrate] for [duration] ms\n");
				  printf("mm [speed][turnrate][duration]:\tmove at [speed]+[turnrate] for [duration] ms\n");
			  }
			  else if(strcmp(cmd,"motion")==0)
			  {
			  	printMotionList(motionList);
			  }
			  else
			  {
				  printf("unrecognised command\n");
			  }
			  break;
			case 3:
			  if (strcmp(cmd,"move")==0)
			  {
				 tail++;
				  if(tail==MAX_MOTION)
					  tail=0;
				  if(tail==header)
				  {
					  printf("list full, last motion will lost\n");
					  if(tail==0)
						  tail =MAX_MOTION-1;
					  else
						  tail = tail - 1;
					  break;
				  }
				  printf("move at speed %d for %d ms\n",arg1,arg2);
				  motionList[tail].speed=arg1;
				  motionList[tail].turnrate=0;
				  motionList[tail].duration=arg2;
				  

			  }
			  else if(strcmp(cmd,"turn")==0)
			  {
				  tail++;
				  if(tail==MAX_MOTION)
					  tail=0;
				  if(tail==header)
				  {
					  printf("list full, last motion will lost\n");
					  if(tail==0)
						  tail =MAX_MOTION-1;
					  else
						  tail = tail - 1;
					  break;
				  }
				  printf("turn at speed %d for %d ms\n", arg1, arg2 );
				  motionList[tail].speed=0;
				  motionList[tail].turnrate=arg1;
				  motionList[tail].duration=arg2;
				 
			  }
			  else
			  {
				  printf("unrecognised command\n");

			  }
			  break;
			  case 4:
			  if (strcmp(cmd,"mm")==0)
			  {
				 tail++;
				  if(tail==MAX_MOTION)
					  tail=0;
				  if(tail==header)
				  {
					  printf("list full, last motion will lost\n");
					  if(tail==0)
						  tail =MAX_MOTION-1;
					  else
						  tail = tail - 1;
					  break;
				  }
				  printf("move at speed %d \t turn rate %d\tfor %d ms\n",arg1,arg2, arg3);
				  motionList[tail].speed=arg1;
				  motionList[tail].turnrate=arg2;
				  motionList[tail].duration=arg3;
				  

			  }
			   else
			  {
				  printf("unrecognised command\n");

			  }
			  break;
			default:
			  printf("unrecognised command\n");
			  break;
		}
	}
	else
	{
		printf("unrecognised command\n");
	}

	return 1;

}

#define WHEEL_SEPERATION 0.052
#define WHEEL_DIAMETER 0.04
void setSpeed2(int speed, int turnrate)
{
	int lspeed=0,rspeed=0;

	lspeed = speed + turnrate * WHEEL_SEPERATION * 1000 /(360.0 * WHEEL_DIAMETER);
	rspeed = speed - turnrate * WHEEL_SEPERATION * 1000 /(360.0 * WHEEL_DIAMETER);
#ifdef EPUCK
	setSpeed(lspeed, rspeed);
	update(fd);
#else
	printf("\n----left wheel: %d right wheel %d\nepuck>",lspeed,rspeed);
	fflush(stdout);
#endif
}

void printMotionList(struct motion_t * list)
{
	int i=0;
	for(i=0;i<MAX_MOTION;i++) {
	 	printf("%d: [%d %d %d]", i, motionList[i].speed, motionList[i].turnrate, motionList[i].duration);
	 	if(i==header)
		 	printf("-->");
	 	if(i==tail)
	 		printf("<--");
	 	printf("\n");
	}
}

