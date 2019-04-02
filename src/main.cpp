#include "PID.h"
#include <mbed.h>

InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D12);
PwmOut ignitionOut(D5);

#define ledpin LED3
#define Pi 3.1415

const double diameter = 1.3333333; // in feet
const double radius = 0.666666;    // in feet
const double circ = diameter * Pi;

double rightSpeed = 0;
double leftSpeed = 0;
double rearSpeed = 0;

double frontSpeed = 0;

unsigned short delta = 0;
unsigned char highByte = 0;
unsigned char lowByte = 0;

DigitalOut out(ledpin);

// CAN can1(D10, D2); // CAN bus pins
PID controller(1.0, 2.0, 2.0, 0.02); // this is undercompensating

Timer tf1;
Timer tf2;
Timer r;
// Timer accelTimer;

long time1 = 1;
long time2 = 1;
long time3 = 1;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

double speed;

double calculateSpeed(double ticks, int micros);

void right_triggered() {

  out = !out;

  if (rightCount == 0) {
    tf1.start();
    rightCount++;
  }

  else if (rightCount == 5) {

    tf1.stop();

    time1 = tf1.read_us();
    rightSpeed = calculateSpeed(5.0, time1);

    rightCount = 0;
    tf1.reset();
  } else {
    rightCount++;
  }
}

void left_triggered() {

  out = !out;

  if (leftCount == 0) {
    tf2.start();
    leftCount++;
  }

  else if (leftCount == 5) {

    tf2.stop();

    time2 = tf2.read_us();
    leftSpeed = calculateSpeed(5.0, time2);

    leftCount = 0;
    tf2.reset();
  } else {
    leftCount++;
  }
}

void rear_triggered() {

  out = !out;

  if (rearCount == 0) {
    r.start();
    rearCount++;
  }

  else if (rearCount == 25) {

    r.stop();

    time3 = r.read_us();
    rearSpeed = calculateSpeed(25.0, time3);
    // rearAccel = calculateAcceleration(prev, speed, time3);

    rearCount = 0;
    r.reset();
  } else {
    rearCount++;
  }
}

double calculateSpeed(double ticks, int micros) {

  double revs;
  double speed;
  double seconds = micros / 1000000.0;

  double ticksPerSecond = (ticks / seconds); // ticks per second

  if (ticks == 5) {
    revs = ticksPerSecond / 25.0; // revs per second
  } else {
    revs = ticksPerSecond / 100.0;
  }

  speed = revs * 2.8553; // 2.8553 is the number that converts rev/s to linear
                         // speed (wheel size dependent)

  return speed;
}

/*
double calculateAcceleration(double prev, double newSpeed, double t) {

    double t2 = t/1000000.0;

    double accel = (newSpeed - prev)/(t2);

    return accel;
}
*/

int main() {

  float pidResult = 1.0;

  left.mode(PullUp);
  right.mode(PullUp);
  rear.mode(PullUp);

  right.rise(&right_triggered);
  left.rise(&left_triggered);
  rear.fall(&rear_triggered);

  controller.setInputLimits(0.0, 40.0);
  controller.setOutputLimits(1.0, 750.0);
  controller.setMode(0); // manual mode
  // controller.setTunings(3.0,10.0,1.0);

  controller.setSetPoint(
      8.0); // mph difference for testing, should be a percentage

  ignitionOut.period_ms(1000);
  ignitionOut.write(0.5);

  while (1) {

    wait_ms(20);

    // printf("%s","Rear speed: ");
    // printf("%f\n",rearSpeed);

    if (leftSpeed >= rightSpeed) {
      frontSpeed = leftSpeed;
    } else {
      frontSpeed = rightSpeed;
    }
    // printf("%s","Front speed: ");
    // printf("%f\n\n",frontSpeed);

    /*
    delta = (frontSpeed / rearSpeed) *100;
    printf("%f\n",delta);
    highByte = (delta >> 8) & 0xFF;
    lowByte = delta & 0xFF;
    */

    if (frontSpeed < 2.0) {

      ignitionOut.period(1.0);
      ignitionOut.write(0.5);
    }

    else {
      float diff = abs(rearSpeed - frontSpeed);

      printf("%f\n", diff);
      controller.setProcessValue(diff);
      pidResult = controller.compute();

      // printf("%f\n\n", pidResult);
      float freq = pidResult;
      float period = 1.0 / freq;
      // printf("%f\n", freq);
      // printf("%f\n", period);
      ignitionOut.period(period);
      ignitionOut.write(0.5);
    }
  }
}