#include <iostream>
using namespace std;

#include "robot.h"

Robot::Robot ( ) {
    if(!initRobot(&bot)) {
    	cerr << "Failed to init robot for some reason!" << endl;
    }
}

Robot::~Robot () {
    bot.alive = false;
    if (pthread_join(bot.robot_update_thread, NULL) != 0) {
    	cerr << "Failed to join with update thread, uncleanly exited check robot state!" << endl;
    }
    else {
    	cerr << "Joined with update thread, cleanly exited." << endl;
    }
    free(&bot);
}

void Robot::debug (void) {
    robotDebug(&bot);
}

void Robot::getLED (int id) {

}

void Robot::getIR (int id) {

}

int Robot::getAccX (void) {

}

unsigned long Robot::getSteps (int wheel) {

}
