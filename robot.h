#ifndef	_ROBOT_H
#define	_ROBOT_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "spicomm.h"
#include "support.h"

#define EPUCK 1

#define ON 1
#define OFF 0

/***
 *    Type defs
 ***/

/* Define a Robot type. */
typedef struct robot {
    pthread_t robot_update_thread;
    int spi;
    bool alive;
    int16_t r_wheel_speed;  /* Speed of right wheel */
    unsigned long r_wheel_steps;  /* Total number of steps in the right wheel */
    int16_t r_wheel_last_steps;  /* Number of steps in the right wheel */
    int16_t l_wheel_speed;  /* Speed of left wheel */
    unsigned long l_wheel_steps;  /* Total number of steps in the left wheel */
    int16_t l_wheel_last_steps;  /* Number of steps in the left wheel */
    int16_t led[8];         /* Ring LED states */
    int16_t bodyled;        /* Body LED state */
    int16_t frontled;       /* Front LED state */
    int16_t ir_range[8];    /* IR Ranges */
    int16_t ir_amb[8];      /* Ambient IR */
    int16_t ir_front_right;
    int16_t ir_rear_right;
    int16_t ir_rear_left;
    int16_t ir_front_left;
    int16_t ir_crowding;
    int16_t x_pos;
    int16_t y_pos;
    int16_t z_pos;
    int16_t x_acl;          /* Accelerometer X component */
    int16_t y_acl;          /* Accelerometer Y component */
    int16_t z_acl;          /* Accelerometer Z component */
    int16_t battery_lvl;    /* Battery level */
} robot;


/***
 *    Function prototypes
 ***/

/* Start up SPI comms and the robot update thread once comms are established. */
bool initRobot (robot * bot);

/* Get the robot state as a string. Assigns memory */
char * robotToString (robot * bot);

/* Get the robot state as a string. Assigns memory */
void robotDebug (robot * bot);

/* Get the robot state as a string. Used as a thread */
void * updateRobotState (void * ptr);

void getRobotIR(robot * bot);
void getRobotAcc(robot * bot);
void getRobotSteps(robot * bot);

/* Sleep a thread whilst a robot is crowded in the swarm. Don't waste energy! */
int16_t waitWhileCrowded(robot * bot);

int waitForRandDecay(int ticks);

/* Stop robot safely and close down SPI comms and release any memory. */
bool closeRobot (robot * bot);

#ifdef __cplusplus
}
using namespace std;

class Robot {
    robot bot;   
  public:
    Robot ();
    virtual ~Robot ();
    void debug (void);
    void getLED (int id);
    void getIR (int id);
    int getAccX (void);
    unsigned long getSteps (int wheel);
};


#endif 

#endif
