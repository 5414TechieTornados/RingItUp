#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     right,         tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     left,          tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     ramp,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     arm,           tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    finger,               tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    claw,                 tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Define class variables

//Max amount of power to be applied to motors
const int MAX_MOTOR = 40;
//Max amount of power to be applied to the ramp motor
const int MAX_RAMP_MOTOR = 80;
//The max value to come from the joysticks
const float MAX_JOYSTICK = 128.0;
//The amount of power to apply to the arm motor
const int ARM_MOTOR_POWER = 30;
const int ARM_MOTOR_POWER_DOWN = 15;
//The max value to apply to rasing the claw servo
const int MAX_UP_CLAW_VALUE = 256;
//The max value to apply to lowering the claw servo
const int MAX_DOWN_CLAW_VALUE = 0;

//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                           Tele-Operation Mode Code Template
//
// This file contains a template for simplified creation of an tele-op program for an FTC
// competition.
//
// You need to customize two functions with code unique to your specific robot.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    initializeRobot
//
// Prior to the start of tele-op mode, you may want to perform some initialization on your robot
// and the variables within your program.
//
// In most cases, you may not have to add any code to this function and it will remain "empty".
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool turbo = false;

void initializeRobot()
{
  servoTarget[claw] = 115;
  // Place code here to initialize servos to starting positions.
  // Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.

  return;
}

//Basic function for moving the robot
int getMotorOutput(int joystickValue){

  float ratio = joystickValue / MAX_JOYSTICK;
  int outputValue = ratio * MAX_MOTOR;

  return outputValue;
}

//More advanced function for moving the robot
int getLogarithmicMotorOutput(int joystickValue){

  //used to determine whether the stick was pressed up or down
  int directionValue = joystickValue / abs(joystickValue);
  float ratio = (joystickValue * joystickValue) / (MAX_JOYSTICK * MAX_JOYSTICK);
  int outputValue;
	if(turbo){
  	outputValue = (ratio * (MAX_MOTOR * MAX_MOTOR)) * directionValue;
	}
	else{
		outputValue = (ratio * (20 * 20)) * directionValue;
	}
  return outputValue;
}

//Gets the values on the sticks and passes it to the motor methods
void moveRobot(){

    motor[left] = getLogarithmicMotorOutput(joystick.joy1_y1);
    motor[right] = getLogarithmicMotorOutput(joystick.joy1_y2);
}


//Method used to deploy ramp
void deployRamp(){
  //if button is pressed apply positve power
  if(joy2Btn(7))
  {
    motor[ramp] = MAX_RAMP_MOTOR ;
  }
  //if button is pressed apply negative power
  else if(joy2Btn(8))
  {
    motor[ramp] = MAX_RAMP_MOTOR * -1;
  }
  //what is this for?
  else if(joy2Btn(9))
  {
    motor[ramp] = MAX_RAMP_MOTOR;
  }
  //if no button relating the ramp is pressed apply no power
  else
  {
    motor[ramp] = 0;
  }
}

//Method raises and lowers the finger with button push depending upon the current servo values
void moveFinger(){
	if(joy1Btn(2)){
  	servoTarget[finger] = 220;
	}
	else if(joy1Btn(4)){
		servoTarget[finger] = 0;
	}
}

//Method increments or decrements the claw based on the controller input compared against the max values
void moveClaw(){
  //Checks to make sure the buttons are pressed, and that they aren't being raised or lowered over their max ranges
  //CHECK ACTUAL BUTTON NUMBERS
  if(joy1Btn(1)){
    servoTarget[claw] = ServoValue(claw) + 3;
  }
  else if(joy1Btn(3)){
    servoTarget[claw] = ServoValue(claw) - 3;
  }
  moveFinger();
}


void autoUp(){
	motor[ramp] = MAX_MOTOR;
	servoTarget[claw] = 250;
	motor[arm] = 45 * -1;
	wait1Msec(700);
	motor[arm] = 0;
	motor[ramp] = 0;
}

void autoDown(){
	servoTarget[claw] = 115;
	motor[arm] = ARM_MOTOR_POWER_DOWN;
	wait1Msec(550);
	motor[arm] = 0;
}

void moveArm(){
  // if button is pressed apply positive power
  if(joy1Btn(5))
  {
    motor[arm] = ARM_MOTOR_POWER_DOWN;
  }
  //if button is pressed apply negative power
  else if(joy1Btn(6))
  {
    motor[arm] = ARM_MOTOR_POWER * -1;
  }
  else if(joy2Btn(4)){
  	autoUp();
	}
	else if(joy2Btn(2)){
		autoDown();
	}
  else
  {
    motor[arm] = 0;
  }
  moveClaw();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         Main Task
//
// The following is the main code for the tele-op robot operation. Customize as appropriate for
// your specific robot.
//
// Game controller / joystick information is sent periodically (about every 50 milliseconds) from
// the FMS (Field Management System) to the robot. Most tele-op programs will follow the following
// logic:
//   1. Loop forever repeating the following actions:
//   2. Get the latest game controller / joystick settings that have been received from the PC.
//   3. Perform appropriate actions* based on the joystick + buttons settings. This is usually a
//      simple action:
//      *  Joystick values are usually directly translated into power levels for a motor or
//         position of a servo.
//      *  Buttons are usually used to start/stop a motor or cause a servo to move to a specific
//         position.
//   4. Repeat the loop.
//
// Your program needs to continuously loop because you need to continuously respond to changes in
// the game controller settings.
//
// At the end of the tele-op period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void checkTurbo(){
	if(joy1Btn(10)){
		turbo = true;
}
	else if(joy1Btn(9)){
		turbo = false;
	}
}

task main()
{
  initializeRobot();
  bFloatDuringInactiveMotorPWM = false;
  waitForStart();   // wait for start of tele-op phase

  //basic instructions for robot
  while (true){
  	getJoystickSettings(joystick);
    moveRobot();
    moveArm();
    deployRamp();
  }
}
