void exercise1();
void exercise2();
void stop();

void rotate( int deg );
void move( int ncm );

int x = 0;
int y = 0;
int rotation = 0;

float rotationsForCm = 36.35; //number of rotations required for a cm
float rotationsFordeg = 6.63; //number of rotations required to turn for a degree

int power = 40;

task main(){
  nMotorPIDSpeedCtrl[motorA] = mtrSpeedReg;
  nMotorPIDSpeedCtrl[motorC] = mtrSpeedReg;
  //move( 40 );
  //exercise1();
  exercise2();
}

void exercise1(){
  move( 40 );
  move( -40 );
  rotate( 90 );
  rotate( -90 );
}

void exercise2(){
  int i;
  for(i = 0; i < 4; i++){
    move( 40 );
    rotate( 90 );
  }
}

int cm2rotations( int ncm ){
  return ncm * rotationsForCm;
}

int deg2rotations( int deg ){
  return deg * rotationsFordeg;
}

void updatePosition( int rotations ){
  // Uncomment to track only a single dot
  //nxtClearPixel( 10 + x/200, 10 + y/200 );
  x += cosDegrees( rotation ) * abs(rotations);
  y += sinDegrees( rotation ) * abs(rotations);
  nxtSetPixel( 10 + x/720, 10 + y/720  );
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
  while(nMotorRunState[motorA] != runStateHoldPosition && nMotorRunState[motorA] != runStateIdle){
  }

  rotation += deg;
  stop();
}

void move( int ncm ){
  int rotations = cm2rotations( abs(ncm) );
  nMotorEncoder[motorA] = 0;
  nMotorEncoder[motorC] = 0;
  
  int sign = (ncm < 0) ? -1 : 1;
  
  nSyncedMotors = synchAC;
  nMotorEncoderTarget[motorA] = rotations;
  motor[motorA] = sign * power;
  
  int prevRotations = 0;
  
  while(nMotorRunState[motorA] == runStateRunning){
    curRotations = abs(nMotorEncoder[motorA]);
    updatePosition( curRotations - prevRotations );
    prevRotations = curRotations;
  }

  stop();
  
  nSyncedMotors = synchNone;
}
