#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     motorMux,       sensorI2CMuxController)
#pragma config(Sensor, S2,     colorSensor,    sensorCOLORFULL)
#pragma config(Sensor, S3,     IRSeeker,       sensorI2CCustom)
#pragma config(Sensor, S4,     gyroSensor,     sensorI2CHiTechnicGyro)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     motor1,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     motor2,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     motor3,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motor4,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     armMotor,      tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     flagMotor,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_2,    servo2,               tServoNone)
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
	motor[motor1] = 0;
  motor[motor2] = 0;
	motor[motor3] = 0;
	motor[motor4] = 0;
}
void rightSidePower(int power) {
	motor[motor2] = power;
	motor[motor4] = power;
}
void leftSidePower(int power) {
	motor[motor1] = power;
	motor[motor3] = power;
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
	if (joy1Btn(2) == 1) {
		scaler = 4;
		PlaySound(soundBeepBeep);
	}
	if (joy1Btn(3) == 1) {
		scaler = 2;
		PlaySound(soundBeepBeep);
	}
	if (joy1Btn(4) == 1) {
		scaler = 1;
		PlaySound(soundBeepBeep);
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
		rightWheelSpeed += turningAmount;
	if (turningAmount < 0)
		leftWheelSpeed -= turningAmount;

	motor[motor1] = leftWheelSpeed;
	motor[motor3] = leftWheelSpeed;
	motor[motor2] = rightWheelSpeed;
	motor[motor4] = rightWheelSpeed;
}
int lastEncoderPos = 200;
void arm() {
	// Button to reset encoder to 0 at bottom
	// Button *labeled* 10
	if (joystick.joy2_Buttons == 512) {
		nMotorEncoder[armMotor] = 0;
	}

	// Function for the 2nd gamepad that controls the arm
	int armAmount = joystick.joy2_y1;
	int clawAmount = joystick.joy2_y2;

	if (abs(armAmount) < threshold)
		armAmount = 0;
	if (abs(clawAmount) < threshold)
		clawAmount = 0;

	const int centerArmPosition = 1000;
	const short morePowerDivision = 2;
	const short lessPowerDivision = 6;
	if (armAmount > 0 && nMotorEncoder[armMotor] < centerArmPosition) {
		// Apply more power
		armAmount /= morePowerDivision;
	}
	if (armAmount > 0 && nMotorEncoder[armMotor] >= centerArmPosition) {
		// Apply less power
		armAmount /= lessPowerDivision;
	}
	if (armAmount < 0 && nMotorEncoder[armMotor] < centerArmPosition) {
		// Apply less power
		armAmount /= lessPowerDivision;
	}
	if (armAmount < 0 && nMotorEncoder[armMotor] >= centerArmPosition) {
		// Apply more power
		armAmount /= morePowerDivision;
	}
	motor[armMotor] = armAmount;

	// Hold motor in constant position
	const int armThreshold = 5;
	const int powerToApply = 5;
	int currentEncoderPos = nMotorEncoder[armMotor];
	int rotationSinceLastMoved = currentEncoderPos - lastEncoderPos;
	if (abs(rotationSinceLastMoved) > armThreshold) {
		if (rotationSinceLastMoved > 0)
			motor[armMotor] += powerToApply;
		if (rotationSinceLastMoved < 0)
			motor[armMotor] -= powerToApply;
	}

	if (armAmount != 0)
		lastEncoderPos = currentEncoderPos;

	int degreeChange = 0;
	if (clawAmount < 0)
		degreeChange = -3;
	if (clawAmount > 0)
		degreeChange = 3;
	servoChangeRate[servo1] = 20;
	servoChangeRate[servo2] = 20;

	servo[servo1] = ServoValue[servo1] + degreeChange;
	servo[servo2] = ServoValue[servo2] - degreeChange;

	// Flag raiser
	int flagMotorSpeed = 0;
	if (joy2Btn(5) == 1) {
		flagMotorSpeed = 10;
	}
	if (joy2Btn(6) == 1) {
		flagMotorSpeed = -10;
	}
	motor[flagMotor] = flagMotorSpeed;
}
void datalogging() {
	eraseDisplay();
	int encoderValue = nMotorEncoder[armMotor];
  nxtDisplayTextLine(2, "Encoder: %d", encoderValue);
  nxtDisplayTextLine(4, "Servo1: %d", ServoValue[servo1]);
  nxtDisplayTextLine(5, "Servo2: %d", ServoValue[servo2]);
}

task main() {
	//nMotorEncoder[armMotor] = 1500; // Staight up
	// Closed to begin with
	servo[servo1] = 270;
	servo[servo2] = 0;
	waitForStart();
	/*if (nMotorEncoder[armMotor] >= 1000) {
		// Arm is layed back
		while (nMotorEncoder[armMotor] >= 1000) {
			motor[armMotor] = -75;
		}
		while (nMotorEncoder[armMotor] > 200) {
			motor[armMotor] = -10;
		}
		motor[armMotor] = 0;
	}*/
	while (true) {
		bFloatDuringInactiveMotorPWM = false;
		getJoystickSettings(joystick);
		driver();
		arm();
		datalogging();
		alive();
	}
}
