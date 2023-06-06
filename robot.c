#include "robot.h"

bool initRobot (robot * bot) {

    /* Zero what needs to be 0 in the robot */
    memset(bot, 0, sizeof *bot);

    #ifdef EPUCK
    bot->spi = init_spi();
    if (bot->spi == -1) {
        fprintf(stderr,"Failed to initialise SPI interface.\n");
        return false;
    }
    #endif

    if(pthread_create(&bot->robot_update_thread, NULL, updateRobotState, bot)) {
        fprintf(stderr, "Failed to create robot update thread, exiting!\n");
    	return false;
    }
    else {
	bot->alive = true;
    	return true;
    }
}

char * robotToString(robot * bot) {

	char * string;

	if (asprintf(string,
		"*epuck state\n\tlwheel: %d\n\trwheel: %d\n\x0"
	, bot->l_wheel_speed, bot->r_wheel_speed ) == -1) {
		string = NULL;
	}
	return string;
}

void robotDebug(robot * bot) {
	printf("*epuck state\n\tlwheel: %d\n\trwheel: %d\n"
	, bot->l_wheel_speed, bot->r_wheel_speed );
	printf("\tTotal Steps: %lul\t%lur\n", bot->l_wheel_steps, bot->r_wheel_steps);
	printf("\tLast Steps: %dl\t%dr\n", bot->l_wheel_last_steps, bot->r_wheel_last_steps);
	printf("\tX Y Z: %dx\t%dy\t%dz\n", bot->x_acl, bot->y_acl, bot->z_acl);
	int i;
	for(i=0;i<8; i++){printf("\tIR%d: %d\n",i+1,bot->ir_range[i]);}
	printf("\tIR Avg: fr%d, rr%d, rl%d, fl%d\n", bot->ir_front_right, bot->ir_rear_right, bot->ir_rear_left, bot->ir_front_left);
	printf("\tIR Crowding: %d\n", bot->ir_crowding);
	printf("\tTotal Steps: %dl\t%dr\n", bot->l_wheel_steps, bot->r_wheel_steps);
}

void * updateRobotState (void * ptr) {

	extern struct rxbuf_t msgRX; /*TODO From spicomm need to clean this up! */
	
	robot * bot = (robot *)ptr;
	robot prestate = *bot;

	usleep(2000);

	#ifdef EPUCK
		set_board_led(1, ON);
	#endif

	while (bot->alive) {
		/*Change state here */
		if ((bot->l_wheel_speed != prestate.l_wheel_speed) || (bot->r_wheel_speed != prestate.r_wheel_speed)) {
   			printf("Updating Robot! %d %d\n", bot->l_wheel_speed, bot->r_wheel_speed);
   			setSpeed(bot->l_wheel_speed, bot->r_wheel_speed);
			prestate = *bot;
			update(bot->spi);
   		}

		getRobotIR(bot);
		getRobotAcc(bot);
		getRobotSteps(bot);

		//waitUntil(1); //wait for a ms
		usleep(100);
	}

	closeRobot(bot);

	pthread_exit();

}

void getRobotIR(robot * bot) {

	extern struct rxbuf_t msgRX; /*TODO From spicomm need to clean this up! */

	/*Get stuff back here */
	bot->ir_crowding = 0;
	int i;
	for (i = 0; i<8; i++) {
		bot->ir_range[i] = msgRX.ir[i];
		bot->ir_crowding += msgRX.ir[i];
	}
	bot->ir_crowding /= 8;
	bot->ir_front_right = (bot->ir_range[1] + bot->ir_range[2] + bot->ir_range[3]) / 3;
	bot->ir_rear_right = (bot->ir_range[3] + bot->ir_range[4]) / 2;
	bot->ir_rear_left = (bot->ir_range[5] + bot->ir_range[6]) / 2;
	bot->ir_front_left = (bot->ir_range[6] + bot->ir_range[7] + bot->ir_range[8]) / 3;
}

void getRobotAcc(robot * bot) {

	extern struct rxbuf_t msgRX; /*TODO From spicomm need to clean this up! */

	bot->x_acl = msgRX.acc[0];
	bot->y_acl = msgRX.acc[1];
	bot->z_acl = msgRX.acc[2];
}

void getRobotSteps(robot * bot) {

	extern struct rxbuf_t msgRX; /*TODO From spicomm need to clean this up! */

	//printf("Message holds: %d %d\n",msgRX.tacl,msgRX.tacr);
	bot->l_wheel_last_steps = msgRX.tacl;
	bot->l_wheel_steps += bot->l_wheel_last_steps;
	bot->r_wheel_last_steps = msgRX.tacr;
	bot->r_wheel_steps += bot->r_wheel_last_steps;
}

/**
 *Sleep a thread whilst the robot is crowded.
 */
int16_t waitWhileCrowded(robot * bot) {
	int wait = -1;
	while ( (wait != 0) && (bot->ir_crowding > 0) ) {
		printf("Waiting for %d ticks\n",wait);
		wait = waitForRandDecay(wait);
	}
	return bot->ir_crowding;
}

/**
 * Sleep a thread for a certain amount of ticks and return decayed time.
 */
int waitForRandDecay(int ticks) {
	if (ticks == -1) {
		ticks = rand() % 1000;
	}
	else {
		ticks *= expf( (float) -2*ticks );
	}
	printf("Waiting for %d ticks!\n",ticks);
	//waitFor(ticks);
	return ticks;
}

/**
 * Set the robot to being dead, and close down comms to the dsPIC!
 */
bool closeRobot (robot * bot) {

    if (bot->alive) {
        bot->alive = false;
    }

    fprintf(stderr, "Closing robot cleanly...\n");
    setSpeed(0,0);
    update(bot->spi);
    set_board_led(1, OFF);
    reset_robot(bot->spi);

    return true;
}
