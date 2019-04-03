// ROTATING BUFFER TEST
// MEMORY WRITE TEST

#include "PID.h"
#include <mbed.h>

InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D12);
PwmOut ignitionOut(D5);

#define Pi 3.1415

const double diameter = 1.3333333; // in feet
const double radius = 0.666666;    // in feet
const double circ = diameter * Pi;

double rightSpeed = 0;
double leftSpeed = 0;
double rearSpeed = 0;

// Rotating tickTime buffer
double rightTickBuffer[10];
double leftTickBuffer[10];
double rearTickBuffer[10];

int PWMFreqStorage[1000];
double speedDeltaStorage[1000];

double frontSpeed = 0;

unsigned short delta = 0;
unsigned char highByte = 0;
unsigned char lowByte = 0;

DigitalOut led(LED1);

// CAN can1(D10, D2); // CAN bus pins
PID controller(1.0, 2.0, 2.0, 0.02); // this is undercompensating

Timer rightTimer;
Timer leftTimer;
Timer rearTimer;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

double speed;

double calculateSpeed(double ticks, int micros);

void writeToFlash();

// ISRs
void right_triggered() {}

void left_triggered() {}

void rear_triggered() {}

int main() {
  left.mode(PullUp);
  right.mode(PullUp);
  rear.mode(PullUp);

  right.rise(&right_triggered);
  left.rise(&left_triggered);
  rear.fall(&rear_triggered);

  ignitionOut.period_ms(1000); // 1 second
  ignitionOut.write(0.5);

  while (1) {
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

  speed = revs * 2.8553; // Magic number

  return speed;
}
