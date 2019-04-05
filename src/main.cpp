// ROTATING BUFFER TEST

#include <mbed.h>

// Prototypes
void right_triggered();
void left_triggered();
void rear_triggered();

const int BUFF_SIZE = 10;

long rightTickBuff[BUFF_SIZE];
long leftTickBuff[BUFF_SIZE];
long rearTickBuff[BUFF_SIZE];

double rightSpeed = 0.0;
double leftSpeed = 0.0;
double rearSpeed = 0.0;
double groundSpeed = 0.0; // Fastest front

Timer tRight;
Timer tLeft;
Timer tRear;

// GPIOs
DigitalOut led(LED1);

InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D5);

Serial ser(D1, D0);

PwmOut ignitionOut(D6);

// Function Prototypes
double calcSpeed(long inputBuf[], int ticksPerRev);
void addToBuffer(long inputBuf[], long val);
void printArray(long buf[]);
void wipeBuffers();

int main() {
  ser.baud(115200);

  right.mode(PullUp);
  left.mode(PullUp);
  rear.mode(PullUp);

  right.fall(&right_triggered);
  left.fall(&left_triggered);
  rear.fall(&rear_triggered);

  while (1) {
    wait_ms(250);

    rightSpeed = calcSpeed(rightTickBuff, 25);
    leftSpeed = calcSpeed(leftTickBuff, 25);
    rearSpeed = calcSpeed(rearTickBuff, 100);

    if (rightSpeed > leftSpeed) {
      groundSpeed = rightSpeed;
    } else {
      groundSpeed = leftSpeed;
    }

    if (tRight.read() > 0.5 && tLeft.read() > 0.5) {
      wipeBuffers();
    }

    ser.printf("\nFront Speed: %.2f MPH", groundSpeed);
    ser.printf("\nRear Speed: %.2f MPH\n", rearSpeed);
  }
}

// ----- Methods -----

// ISRs
void right_triggered() {
  tRight.stop();
  addToBuffer(rightTickBuff, tRight.read_us());
  tRight.reset();
  tRight.start();

  led = !led;
}

void left_triggered() {
  tLeft.stop();
  addToBuffer(leftTickBuff, tLeft.read_us());
  tLeft.reset();
  tLeft.start();

  led = !led;
}

void rear_triggered() {
  tRear.stop();
  addToBuffer(rearTickBuff, tRear.read_us());
  tRear.reset();
  tRear.start();

  led = !led;
}

// Calculates speed in MPH based on 5 ticks
// Returns speed as a double
// Specify ticks per rev (25 for front, 100 for rear)
double calcSpeed(long inputBuf[], int ticksPerRev) {
  long double buffTotal = 0.0;

  // Always only use the last 5 parts of the buffer
  for (int i = BUFF_SIZE - 5; i < BUFF_SIZE; i++) {
    buffTotal += (long double)inputBuf[i];
  }

  // Calculate the wheelspeed using the square wave freq
  double avgTickPeriod = (buffTotal / 5.0) / 1000000.0;
  double avgTickFreq = 1.0 / avgTickPeriod; // In HZ
  double wheelRPM = avgTickFreq / (double)ticksPerRev;
  // double speedFPS = avgTickFreq * circumference;
  double speedMPH = wheelRPM * 2.8553; // Magic number

  if (speedMPH > 150) {
    speedMPH = 0;
  }

  return speedMPH;
}

// Shift values in the buffer over 1
// Add in the new value to buf[BUFF_SIZE]
void addToBuffer(long inputBuf[], long val) {
  for (int i = 0; i < BUFF_SIZE - 1; i++) {
    inputBuf[i] = inputBuf[i + 1];
  }
  inputBuf[BUFF_SIZE - 1] = val;
}

// Prints out the buffer arrays
void printArray(long buf[]) {
  for (int i = 0; i < BUFF_SIZE; i++) {
    printf("% ld", buf[i]);
  }
}

void wipeBuffers() {
  for (int i = 0; i < BUFF_SIZE; i++) {
    rightTickBuff[i] = 0;
    leftTickBuff[i] = 0;
    rearTickBuff[i] = 0;
  }
}