/****
 *
 * Produce a random walk motion in epucks.
 *
 * Non-standard Dependencies:
 *     robot   - Matt Oates
 *     spicomm - Wenguo Liu
 *     support - Wenguo Liu
 *
 * Edit History:
 *     2009-11-23 - Matt Oates - Frist features added.
 *
 ****/

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
#include "robot.h"

pthread_t timer_thread;

unsigned long lapsed_time = 0;
bool keep_running = true;

void * timerThread(void * startingFrom);
void waitFor(int interval);

void handlerSigAlarm(int dummy);
void handlerSigTerm(int dummy);

robot epuck;

/**
 *      Main Control Loop
 **/
int main(int argc, char*argv[]) {
	/* From Robot.h */

	//Set signal handler to capture "ctrl+c" event
    if (signal(SIGINT, handlerSigTerm) == SIG_ERR) {
        printf("signal(2) failed while setting up for SIGINT");
        return EXIT_FAILURE;
    }
    if (signal(SIGTERM, handlerSigTerm) == SIG_ERR) {
        printf("signal(2) failed while setting up for SIGTERM");
        return EXIT_FAILURE;
    }

    //Set signal handler to capture timer alarm event
    if (signal(SIGALRM, handlerSigAlarm) == SIG_ERR) {
        printf("signal(2) failed while setting up for SIGALRM");
        return EXIT_FAILURE;
    }

	/* Create a timer interval (tick) for every 10 ms (the best that can be done!) */
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 10000;
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 10000;

    /* Start timer to give out SIGALARM callbacks every tick */
    if (setitimer(ITIMER_REAL, &tick, NULL))
    {
        printf("Set timer failed!!\n");
    }

    /*if(pthread_create(&timer_thread, NULL, timerThread, NULL)) {
    	fprintf(stderr, "Failed to create robot timer thread, exiting!\n");
    	return EXIT_FAILURE;
    }*/

    /* Setup the epuck struct and get the update thread running. */
    if(!initRobot(&epuck)) {
    	fprintf(stderr, "Failed to init robot update thread, exiting!\n");
    	return EXIT_FAILURE;
    }
	usleep(2000000);
	/* Randomise with current time before drawing random set.*/
    srand(time(NULL));

	/**
	 * Wait for the epuck to not be crowded anymore.
	 */
	//waitWhileCrowded(&epuck);

    /* Main Event Loop */
    while(keep_running) {

    	/* Make a binary random choice */
    	unsigned int duration = (rand() % 400) + 100;
    	if (random() >= 0.5) {
    		/*Turn Right*/
    		printf("Turning Right\n");
    		epuck.l_wheel_speed = 600;
    		epuck.r_wheel_speed = -600;
    	}
    	else {
    		/*Turn Left*/
    		printf("Turning Left\n");
    		epuck.l_wheel_speed = -600;
    		epuck.r_wheel_speed = 600;
    	}

    	robotDebug(&epuck);
    	//usleep(1000000);
    	waitFor(100);

    	/* Ahead for duration */
    	printf("Going straight for %dms\n", duration);
    	epuck.l_wheel_speed = 600;
    	epuck.r_wheel_speed = 600;

    	robotDebug(&epuck);
    	//usleep(duration);
    	waitFor(duration);

    	if (lapsed_time >= 10000) keep_running = false;
    	else printf("The time is: %ld\n", lapsed_time);
    }

    printf("Stopped doing a random walk.\n");

    epuck.alive = false;
    printf("Waiting for robot to die.\n");
    if (pthread_join(epuck.robot_update_thread, NULL) != 0) {
    	fprintf(stderr,"Failed to join with update thread, uncleanly exited check robot state!\n");
    	return(EXIT_FAILURE);
    }
    printf("Robot died cleanly, exiting.\n");
    /* Should be safe to exit here, dsPIC will have stopped motors. */
    printf("Exiting after %ldms!\n",lapsed_time);
    return(EXIT_SUCCESS);
}

void * timerThread(void * startFrom) {
	extern unsigned long lapsed_time;
	if (startFrom != NULL)
		lapsed_time = *( (unsigned long *) startFrom );
	while(1) {
		usleep(1000);
		lapsed_time = lapsed_time + 1;
	}
}

void waitFor(int interval) {
	unsigned long time_to_wake_up = lapsed_time + interval;
	while(time_to_wake_up >= lapsed_time) {
		usleep(1);
		//printf("It's not %d time to wake up %d\n", time_to_wake_up, lapsed_time);
	}
}

void handlerSigAlarm(int dummy)
{
	lapsed_time+=10;
}

void handlerSigTerm(int dummy) {
	extern unsigned long lapsed_time;
	extern robot epuck;
    printf("Ctrl+C caught, exiting after %ldms!\n",lapsed_time);
#ifdef EPUCK
	closeRobot(&epuck);
#endif
    exit(EXIT_SUCCESS);
}
