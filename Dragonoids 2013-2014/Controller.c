#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     motorMux,       sensorNone)
#pragma config(Sensor, S2,     IRSeeker,       sensorI2CCustom)
#pragma config(Sensor, S3,     gyroSensor,     sensorAnalogInactive)
#pragma config(Sensor, S4,     ultrasonic,     sensorSONAR)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     rearRight,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     rearLeft,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     frontRight,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     frontLeft,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     armMotor,      tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     flagMotor,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    wrist,                tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    flagRaiserExtender,   tServoStandard)
#pragma config(Servo,  srvo_S1_C4_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

/* HOW TO CONTROL THE ROBOT USING THE FOLLOWING CODE:

Push left joystick to turn left, right joystick to turn right, and both to move forward.
If one is pushed more than the other, than the robot will go forward and turn slightly.

Gamepad 1 is driving
Gamepad 2 is arm control
*/
void stopMotors() {
	motor[frontLeft] = 0;
  motor[frontRight] = 0;
	motor[rearLeft] = 0;
	motor[rearRight] = 0;
}
void rightSidePower(int power) {
	motor[frontRight] = power;
	motor[rearRight] = power;
}
void leftSidePower(int power) {
	motor[frontLeft] = power;
	motor[rearLeft] = power;
}

const int threshold = 10;
int scaler = 6;

void driver() {
	// Function for the 1st gamepad that controls driving
	// Joystick values are 8 bit signed ints (-127 to 128) of the data type byte

	// Variable speeds
	if (joy1Btn(1) == 1) {
		scaler = 6;
		PlaySound(soundBeepBeep);
	}
	else if (joy1Btn(2) == 1) {
		scaler = 4;
		PlaySound(soundBeepBeep);
	}
	else if (joy1Btn(3) == 1) {
		scaler = 2;
		PlaySound(soundBeepBeep);
	}
	else if (joy1Btn(4) == 1) {
		scaler = 1;
		PlaySound(soundBeepBeep);
	}
	else {
			ClearSounds();
	}

	// Turning in place
	if (joy1Btn(5) == 1) {
		// Counter clockwise turn
		int powerLevel = 128 / scaler;
		rightSidePower(-powerLevel);
		leftSidePower(powerLevel);
		return;
	}
	if (joy1Btn(6) == 1) {
		// Clockwise turn
		int powerLevel = 128 / scaler;
		rightSidePower(powerLevel);
		leftSidePower(-powerLevel);
		return;
	}

	int forwardAmount = joystick.joy1_y1;
	int turningAmount = joystick.joy1_x2;
	if (abs(forwardAmount) < threshold)
		forwardAmount = 0;
	if (abs(turningAmount) < threshold)
		turningAmount = 0;
	forwardAmount /= scaler;
	turningAmount /= scaler;

	int rightWheelSpeed = forwardAmount;
	int leftWheelSpeed = forwardAmount;
	if (turningAmount > 0)
		leftWheelSpeed += turningAmount;
	if (turningAmount < 0)
		rightWheelSpeed -= turningAmount;

	leftSidePower(leftWheelSpeed);
	rightSidePower(rightWheelSpeed);
}

void arm() {
	int armAmount = joystick.joy2_y1;
	int wristAmount = joystick.joy2_y2;

	if (abs(armAmount) < threshold)
		armAmount = 0;
	if (abs(wristAmount) < threshold)
		wristAmount = 0;

	int maxArmAmountPositive = 50;
	int maxArmAmountNegative = -10;
	if (joy1Btn(1) == 1) {
		maxArmAmountPositive = 50;
		//PlaySound(soundShortBlip);
	}
	else if (joy1Btn(2) == 1) {
		maxArmAmountPositive = 80;
	}

	armAmount /= 4;
	if (armAmount < maxArmAmountNegative)
		armAmount = maxArmAmountNegative;
	if (armAmount > maxArmAmountPositive)
		armAmount = maxArmAmountPositive;
	motor[armMotor] = armAmount;

	int degreeChange = 0;
	if (wristAmount < 0)
		degreeChange = -5;
	if (wristAmount > 0)
		degreeChange = 5;
	servoChangeRate[wrist] = 50;
	servoChangeRate[flagRaiserExtender] = 50;

	servo[wrist] = ServoValue[wrist] - degreeChange;
	//servo[flagRaiserExtender] = ServoValue[flagRaiserExtender] - degreeChange;

	// Flag raiser
	int flagMotorSpeed = 0;
	if (joy2Btn(4) == 1) {
		flagMotorSpeed = -25;
	}
	if (joy2Btn(5) == 1) {
		flagMotorSpeed = -50;
	}
	motor[flagMotor] = flagMotorSpeed;
}
void datalogging() {
	eraseDisplay();
	//int encoderValue = nMotorEncoder[armMotor];
	//nxtDisplayTextLine(2, "Encoder: %d", encoderValue);
	nxtDisplayTextLine(2, "Encoder: %s", "N/A");
	nxtDisplayTextLine(4, "Wrist: %d", ServoValue[wrist]);
	nxtDisplayTextLine(5, "Flag: %d", ServoValue[flagRaiserExtender]);
}

task main() {
	servo[wrist] = 90;
	waitForStart();

	while (true) {
		bFloatDuringInactiveMotorPWM = false;
		getJoystickSettings(joystick);
		driver();
		arm();
		datalogging();
		// RobotC function for keeping the robot on
		alive();
	}
}
