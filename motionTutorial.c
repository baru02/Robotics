#pragma config(Sensor, S1,     touchSensor,         sensorTouch)
#pragma config(Sensor, S2,     sonarSensor,         sensorSONAR)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void exercise();
void pidSetup();
void senseTouch();
void senseSound();
void wallFollow();
void stop();

void rotate( int deg );
void move( int ncm );

int x = 0;
int y = 0;
int rotation = 0;

float rotationsForCm = 28.14; //number of rotations required for a cm
float rotationsFordeg = 3.48; //number of rotations required to turn for a degree

int power = 10;
int distance = 30;

const tSensors touchSensor = (tSensors) S1;
const tSensors sonarSensor = (tSensors) S2;

task main(){
  pidSetup();
  wallFollow();
  //senseSound();
  //senseTouch();
  //exercise();
}

void wallFollow() {
  int currentDistance;
  while(true) {
    currentDistance = SensorValue(sonarSensor) - distance;
    if(currentDistance < 0) {
      motor[motorA] = abs(power/currentDistance);
      motor[motorC] = power;
    }
    if(currentDistance > 0) {  
      motor[motorA] = power;
      motor[motorC] = abs(power/currentDistance);
    }
    else {
      motor[motorA] = power;
      motor[motorC] = power;
    }
  }
}

void senseSound() {
  while(1) {
	  while(SensorValue(sonarSensor) > 10){
			motor[motorA] = power;
			motor[motorC] = power;
		}
		motor[motorA] = -power;
		motor[motorC] = -power;
		wait1Msec(1000);
  }
}

void senseTouch() {
  while(1){
    while(SensorValue(touchSensor) == 0){
	    motor[motorA] = 20;
	    motor[motorC] = 20;
  	}
  	motor[motorA] = -20;
    motor[motorC] = -20;
    wait1Msec(1000);
  }
}

void exercise(){
  int i;
  for(i = 0; i < 4; i++){
    move( 40 );
    rotate( 90 );
  }
}

void pidSetup(){
  nMotorPIDSpeedCtrl[motorA] = mtrSpeedReg;
  nMotorPIDSpeedCtrl[motorC] = mtrSpeedReg;
}

int cm2rotations( int ncm ){
  return ncm * rotationsForCm;
}

int deg2rotations( int deg ){
  return deg * rotationsFordeg;
}

void updatePosition( int rotations ){
  x += cosDegrees( rotation ) * abs(rotations);
  y += sinDegrees( rotation ) * abs(rotations);
  nxtSetPixel( 10 + x/90, 10 + y/90  );
}

void stop(){
  motor[motorA] = 0;
  motor[motorC] = 0;
  nMotorEncoder[motorA] = 0;
  nMotorEncoder[motorC] = 0;
  wait1Msec(1000);
}

void rotate( int deg ){
  int rotations = deg2rotations( abs(deg) );
  nMotorEncoder[motorA] = 0;
  nMotorEncoder[motorC] = 0;

  nMotorEncoderTarget[motorA] = rotations;
  nMotorEncoderTarget[motorC] = rotations;

  int sign = (deg < 0) ? -1 : 1;

  motor[motorA] = sign * -power;
  motor[motorC] = sign * power;
  while(nMotorRunState[motorA] != runStateIdle && nMotorRunState[motorA] != runStateHoldPosition){
  }

  rotation += deg;
  stop();
}

void move( int ncm ){
  int rotations = cm2rotations( abs(ncm) );
  nMotorEncoder[motorA] = 0;
  nMotorEncoder[motorC] = 0;

  int sign = (ncm < 0) ? -1 : 1;

  nMotorEncoderTarget[motorA] = rotations;
  motor[motorA] = sign * power;
  motor[motorC] = sign * power;

  int prevRotations = 0;
  int curRotations = 0;
  while(nMotorRunState[motorA] != runStateIdle && nMotorRunState[motorA] != runStateHoldPosition){
    curRotations = abs(nMotorEncoder[motorA]);
    updatePosition( curRotations - prevRotations );
    prevRotations = curRotations;
  }

  stop();
}
