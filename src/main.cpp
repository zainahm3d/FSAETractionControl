#include <mbed.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Pi 3.1415

DigitalIn right(D10);
DigitalIn left(D11);
DigitalIn rear(D12);

DigitalOut ig(D6);
DigitalOut out(LED3);

const double diameter = 1.3333333; // in feet
const double radius = 0.666666;    // in feet
const double circ = diameter * Pi;

double rightSpeed = 0;
double leftSpeed = 0;
double rearSpeed = 0;
double frontSpeed = 0;

Timer tf1;
Timer tf2;
Timer r;

Timer testTimer;

long time1 = 1;
long time2 = 1;
long time3 = 1;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

int rightPrev = 1;
int leftPrev = 0;
int rearPrev = 0;
int rightCurrent = 0;
int leftCurrent = 0;
int rearCurrent = 0;

double calculateSpeed(double ticks, int micros);

void right_triggered() {
  out = !out;

  float test = tf1.read();
  if (test > 2.5) {
    rightSpeed = 0.0;
    tf1.reset();
  }

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

  float test = tf2.read();
  if (test > 2.5) {
    leftSpeed = 0.0;
    tf2.reset();
  }

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

  float test = r.read();
  if (test > 2.5) {
    rearSpeed = 0.0;
    r.reset();
  }

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

void selectFrontWheel() {

  if (leftSpeed >= rightSpeed) {
    frontSpeed = leftSpeed;
  } else {
    frontSpeed = rightSpeed;
  }
}

void adjustSpeed() {

  float diff = abs(rearSpeed - frontSpeed);

  if (diff > 5.0) {

  } else if (diff > 9.0) {
    ig = 1;
  } else {
    ig = 0;
  }
}

int main() {

  ig = 0;

  testTimer.reset();
  testTimer.start();

  int num = 0;

  right.mode(PullUp);
  left.mode(PullUp);
  rear.mode(PullUp);

  out = 1;

  while (1) {
    // only read the inputs once per loop to save cycles
    rightCurrent = right.read();
    leftCurrent = left.read();
    rearCurrent = rear.read();

    // detect only the rising edges
    if (rightCurrent == 1 && rightPrev == 0) {
      right_triggered();
    }

    if (leftCurrent == 1 && leftPrev == 0) {
      left_triggered();
    }

    if (rearCurrent == 1 && rearPrev == 0) {
      rear_triggered();
    }

    if (leftSpeed >= rightSpeed) {
      frontSpeed = leftSpeed;
    } else {
      frontSpeed = rightSpeed;
    }

    rightPrev = rightCurrent; // set the flags to the current state
    leftPrev = leftCurrent;
    rearPrev = rearCurrent;

    // uncomment to test loops in 1 second
    // num++;
    // if (testTimer.read() >= 1) {
    //   printf("\ndone\n");
    //   printf("\n%d\n", num);
    //   break;
    // }
  }
}
