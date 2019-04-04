// ROTATING BUFFER TEST
// MEMORY WRITE TEST
#include <mbed.h>

InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D12);
// PwmOut ignitionOut(D5);

// Prototypes
void right_triggered();
void left_triggered();
void rear_triggered();

const int BUFF_SIZE = 10;

// Tire Spec (in feet)
const double circumference = 4.189;

long rightTickBuff[BUFF_SIZE];
long leftTickBuff[BUFF_SIZE];
long rearTickBuff[BUFF_SIZE];

double rightSpeed = 0.0;
double leftSpeed = 0.0;
double rearSpeed = 0.0;

Timer tRight;
Timer tLeft;
Timer tRear;

DigitalOut led(LED1);

// Function Prototypes
double calcSpeed(long inputBuf[], int ticksPerRev);
void addToBuffer(long inputBuf[], long val);
void printArray(long buf[]);

int main() {
  left.mode(PullUp);
  right.mode(PullUp);
  rear.mode(PullUp);

  right.fall(&right_triggered);
  left.fall(&left_triggered);
  rear.fall(&rear_triggered);

  while (1) {
    rightSpeed = calcSpeed(rightTickBuff, 25);
    leftSpeed = calcSpeed(leftTickBuff, 25);
    rearSpeed = calcSpeed(rearTickBuff, 100);
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