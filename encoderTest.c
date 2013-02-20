#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  none)
#pragma config(Sensor, S2,     ,               sensorI2CCustom)
#pragma config(Sensor, S4,     irSeeker,       sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     motorC,        tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     motorD,        tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     motorF,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     arm,           tmotorTetrix, PIDControl, encoder)
#pragma config(Servo,  srvo_S1_C2_1,    finger,               tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    claw,                 tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                  Autonomous Mode Code Template
//
// This file contains a template for simplified creation of an autonomous program for an Tetrix robot
// competition.
//
// You need to customize two functions with code unique to your specific robot.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.
#include "hitechnic-irseeker-v2.h" //ir drivers

const float ArmUpRotations = .63;

const float middleStartInches = 9.75;
const float middleTurnRotations = 2;
const float middleFinalInches = 35.5;

const float rightStartInches = 37.75;
const float rightFinalInches = 15.5;

const float leftTurnRotations = 2.82;
const float leftFinalInches = 43;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                      initializeRobot
//
// Prior to the start of autonomous mode, you may want to perform some initialization on your robot.
// Things that might be performed during initialization include:
//   1. Move motors and servos to a preset position.
//   2. Some sensor types take a short while to reach stable values during which time it is best that
//      robot is not moving. For example, gyro sensor needs a few seconds to obtain the background
//      "bias" value.
//
// In many cases, you may not have to add any code to this function and it will remain "empty".
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeRobot()
{
  // Place code here to sinitialize servos to starting positions.
  // Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.
 	servoTarget[finger] = 160;
 	servoTarget[claw] = 0;
 	tHTIRS2DSPMode _mode = DSP_1200;
  return;
}

/*
 *Method used to convert rotations into a value usable by the encoders
 *parameters:
 *rotations - the number of rotations to convert
**/
float convertRotations(float rotations){
	return 1440 * rotations;
}

/*
 *Method used to convert inches into a value usable by the encoders
 *parameters:
 *rotations - the number of inches to convert
**/
float convertInches(float inches){
	return (1440/9.42) * inches;
}

/*
 *Method used to set the arm before moving the robot
**/
void initializeArm(){
	nMotorEncoder[arm] = 0;
	wait1Msec(5);

	nMotorEncoderTarget[arm] = convertRotations(ArmUpRotations);
		motor[arm] = -25;

		while(nMotorRunState[arm] != runStateIdle){
	}

 	motor[arm] = 0;
 	servoTarget[claw] = 100;
}

/*
 *Method used to make the first move of the robot as long as it isn't going to the first rung
 *parameters
 *inches - the number of inches to move
**/
void firstMove(float inches){

	nMotorEncoder[motorC] = 0;
	nMotorEncoder[motorD] = 0;

	wait10Msec(5);

	nMotorEncoderTarget[motorC] = convertInches(inches);
	nMotorEncoderTarget[motorD] = convertInches(inches);

	motor[motorC] = 50;
	motor[motorD] = 50;

	while(nMotorRunState[motorC] != runStateIdle || nMotorRunState[motorD] != runStateIdle){
	}
}

/*
 *Method used to make the turn towards any of the rungs
 *parameters
 *rotations - the number of rotations for the encoders
**/
void leftTurn(float rotations){
	motor[motorC] = 0;
	motor[motorD] = 0;

	nMotorEncoder[motorC] = 0;
	nMotorEncoder[motorD] = 0;
	wait10Msec(5);

	nMotorEncoderTarget[motorD] = convertRotations(rotations);

	motor[motorD] = 50;

	while(nMotorRunState[motorD] != runStateIdle){
	}

	motor[motorD] = 0;
}

/*
 *Method that moves the robot towards the rung to place the ring
 *parameters
 *inches - the number of inches to move the robot
**/
void attackRack(float inches){
	nMotorEncoder[motorC] = 0;
	nMotorEncoder[motorD] = 0;

	wait10Msec(5);

	nMotorEncoderTarget[motorC] = convertInches(inches);
	nMotorEncoderTarget[motorD] = convertInches(inches);

	motor[motorC] = 50;
	motor[motorD] = 50;

	while(nMotorRunState[motorC] != runStateIdle || nMotorRunState[motorD] != runStateIdle){
	}

	motor[motorC] = 0;
	motor[motorD] = 0;

	servoTarget[finger] = 0;

	wait1Msec(50);
}

/*
 *Method that backs away from the rack and turns the robot towards the ring holders
 *
**/
void retreat(){
	motor[motorC] = -50;
	motor[motorD] = -50;
	wait10Msec(200);

	motor[motorC] = -50;
	motor[motorD] = 0;
	wait10Msec(200);


	motor[motorC] = 50;
	motor[motorD] = 50;
	wait10Msec(100);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                          Main Task
//
// The following is the main code for the autonomous robot operation. Customize as appropriate for
// your specific robot.
//
// The types of things you might do during the autonomous phase (for the 2008-9 FTC competition)
// are:
//
//   1. Have the robot follow a line on the game field until it reaches one of the puck storage
//      areas.
//   2. Load pucks into the robot from the storage bin.
//   3. Stop the robot and wait for autonomous phase to end.
//
// This simple template does nothing except play a periodic tone every few seconds.
//
// At the end of the autonomous period, the FMS will autonmatically abort (stop) execution of the program.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

task main()
{
	initializeRobot();
	waitForStart();
	initializeArm();

  int _dirAC = 0;

  wait1Msec(200);

  int sensorDirection = HTIRS2readDCDir(irSeeker);

  if(sensorDirection > 3 && sensorDirection < 6){
  	leftTurn(leftTurnRotations);
  	attackRack(leftFinalInches);
	}
	else{
		firstMove(middleStartInches);

		wait1Msec(500);
		sensorDirection = HTIRS2readDCDir(irSeeker);
		if(sensorDirection < 7){
			leftTurn(middleTurnRotations);
			attackRack(middleFinalInches);
		}
		else{
			firstMove(rightStartInches - middleStartInches);
			leftTurn(middleTurnRotations);
			attackRack(rightFinalInches);
		}
	}
	retreat();
}
