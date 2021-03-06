#pragma config(Sensor, S1,     rightLightSensor,    sensorCOLORGREEN)
#pragma config(Sensor, S2,     rightTouchSensor,    sensorTouch)
#pragma config(Sensor, S3,     leftTouchSensor,     sensorTouch)
#pragma config(Sensor, S4,     leftLightSensor,     sensorCOLORGREEN)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void pidSetup();
void moveToTarget();
void rotate(int deg);
void stop();
void move(int ncm);
int cm2rotations(int ncm);
int deg2rotations(int deg);
void obstacleHandler(int dir);

int power = 10;
float rotationsForCm = 28.14; //number of rotations required for a cm
float rotationsFordeg = 3.48; //number of rotations required to turn for a degree

const tSensors rightLightSensor = (tSensors) S1;
const tSensors rightTouchSensor = (tSensors) S2;
const tSensors leftTouchSensor = (tSensors) S3;
const tSensors leftLightSensor = (tSensors) S4;

bool lightFound = false;
bool handleObstacleLeft = false;
bool handleObstacleRight = false;
bool moving = false;

task checkLightSource(){
    while(true){
      lightFound = ((SensorValue(rightLightSensor) > 20 || 
                     SensorValue(rightLightSensor) > 20) && 
                     abs(SensorValue(rightLightSensor) - 
                         SensorValue(leftLightSensor)) < 10);
    }
}

task findLightSource(){
    if((SensorValue(rightLightSensor) > 20 || 
        SensorValue(rightLightSensor) > 20) &&
        SensorValue(rightLightSensor) > SensorValue(leftLightSensor)){
      motor[motorA] = -power;
      motor[motorC] = power;
    } else if((SensorValue(rightLightSensor) > 20 || 
        SensorValue(rightLightSensor) > 20) &&
        SensorValue(rightLightSensor) < SensorValue(leftLightSensor)) {
      motor[motorA] = power;
      motor[motorC] = -power;
    } else {
      motor[motorA] = power;
      motor[motorC] = -power;
    }
}

task checkObstacles(){
    while(true){
      handleObstacleLeft = SensorValue(rightTouchSensor) == 1;
      handleObstacleRight = SensorValue(leftTouchSensor) == 1;
    }
}

task main() {
    pidSetup();
    StartTask(checkLightSource);
    StartTask(checkObstacles);
    while(true){
      if(!lightFound){ 
        StartTask(findLightSource);
      } else if(handleObstacleLeft){
        obstacleHandler(0);
      } else if(handleObstacleRight){
        obstacleHandler(1);
      } else {
        StopTask(findLightSource);
        moveToTarget();
      }
    }
}

void obstacleHandler(int dir){
  StopTask(checkLightSource);
  move(-10);
  if(dir == 0){
    rotate(-90);
    move(30);
    rotate(90);
    move(10);
  } else {
    rotate(90);
    move(30);
    rotate(-90);
    move(10);
  }
  handleObstacleLeft = false;
  handleObstacleRight = false;
  StartTask(checkLightSource);
}

void moveToTarget(){
  motor[motorA] = power;
  motor[motorC] = power;
}

void pidSetup() {
    nMotorPIDSpeedCtrl[motorA] = mtrSpeedReg;
    nMotorPIDSpeedCtrl[motorC] = mtrSpeedReg;
}

void rotate(int deg) {
    int rotations = deg2rotations(abs(deg));
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorC] = 0;

    nMotorEncoderTarget[motorA] = rotations;
    nMotorEncoderTarget[motorC] = rotations;

    int sign = (deg < 0) ? -1 : 1;

    motor[motorA] = sign * -power;
    motor[motorC] = sign * power;
    while (nMotorRunState[motorA] != runStateIdle && nMotorRunState[motorA] != runStateHoldPosition) {}
    
    stop();
}

void move(int ncm) {
    int rotations = cm2rotations(abs(ncm));
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorC] = 0;

    int sign = (ncm < 0) ? -1 : 1;

    nMotorEncoderTarget[motorA] = rotations;
    motor[motorA] = sign * power;
    motor[motorC] = sign * power;

    while (nMotorRunState[motorA] != runStateIdle && nMotorRunState[motorA] != runStateHoldPosition) {}
    stop();
}

int cm2rotations(int ncm) {
    return ncm * rotationsForCm;
}

int deg2rotations(int deg) {
    return deg * rotationsFordeg;
}

void stop() {
    motor[motorA] = 0;
    motor[motorC] = 0;
    nMotorEncoder[motorA] = 0;
    nMotorEncoder[motorC] = 0;
    wait1Msec(100);
}
