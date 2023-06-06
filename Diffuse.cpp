#include <iostream>
using namespace std;

#include "robot.h"

int main(int argc, char**argv) {
	Robot epuck = Robot();
	cout << "Created the robot object!" << endl;
	sleep(2);
	cout << "Trying to print some debug info from the robot..." << endl;
	int num;	
	for (num=0;num<5;num++) {epuck.debug();}
	delete &epuck;
	cout << "Destroyed the robot object!" << endl;
}
