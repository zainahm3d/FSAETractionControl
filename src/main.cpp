#include <mbed.h>
#include <stdlib.h>
#include <stdio.h>
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

long time1 = 1;
long time2 = 1;
long time3 = 1;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

int leftPrev;
int rightPrev;
int rearPrev;

double calculateSpeed(double ticks, int micros);

void setInitialStates() {

    leftPrev = left.read();
    rightPrev = right.read();
    rearPrev = rear.read();
    
}

void right_triggered()
{
   int x = right.read();
    if ((x == 0 && rightPrev == 1) || (x == 1 && rightPrev == 0)) {

        out = !out;
        rightCount++;
        rightPrev = x;

        // makes sure speed doesnt get stuck when not moving 
        float test = tf1.read();
        if (test > 2.5) {
            rightSpeed = 0.0;
            tf1.reset();
        }

        if (rightCount == 1) {
            tf1.start();
        }

        else if (rightCount == 6)  {

            tf1.stop();

            time1 = tf1.read_us();
            rightSpeed = calculateSpeed(5.0, time1);
       
            rightCount = 0;
            tf1.reset();
        }
       
    }
}

void left_triggered()
{
   int x = left.read();
    if ((x == 0 && leftPrev == 1) || (x == 1 && leftPrev == 0)) {

        out = !out;
        leftCount++;
        leftPrev = x;

        // makes sure speed doesnt get stuck when not moving 
        float test = tf2.read();
        if (test > 2.5) {
            leftSpeed = 0.0;
            tf2.reset();
        }

        if (leftCount == 1) {
            tf2.start();
        }

        else if (leftCount == 6)  {

            tf2.stop();

            time2 = tf2.read_us();
            leftSpeed = calculateSpeed(5.0, time2);
       
            leftCount = 0;
            tf2.reset();
        }
       
    }
}

void rear_triggered()
{
    int x = rear.read();
    if ((x == 0 && rearPrev == 1) || (x == 1 && rearPrev == 0)) {

        out = !out;
        rearCount++;
        rearPrev = x;

        // makes sure speed doesnt get stuck when not moving 
        float test = r.read();
        if (test > 2.5) {
            rearSpeed = 0.0;
            r.reset();
        }

        if (rearCount == 1) {
            r.start();
        }

        else if (rearCount == 26)  {

            r.stop();

            time3 = r.read_us();
            rearSpeed = calculateSpeed(25.0, time3);
       
            rearCount = 0;
            r.reset();
        }
       
    }
    
}

double calculateSpeed(double ticks, int micros)
{

    double revs;
    double speed;
    double seconds = micros / 1000000.0;

    double ticksPerSecond = (ticks / seconds); // ticks per second

    if (ticks == 5)
    {
        revs = ticksPerSecond / 25.0; //revs per second
    }
    else
    {
        revs = ticksPerSecond / 100.0;
    }

    speed = revs * 2.8553; // 2.8553 is the number that converts rev/s to linear speed
                           // (wheel size dependent)
    return speed;
}

void selectFrontWheel() {

    if (leftSpeed >= rightSpeed)
        {
            frontSpeed = leftSpeed;
        }
        else
        {
            frontSpeed = rightSpeed;
        }
}

void adjustSpeed() {

    float diff = abs(rearSpeed - frontSpeed);

    if (diff > 5.0)
    {

    }
    else if (diff > 9.0)
    {
        ig = 1;
    }
    else
    {
        ig = 0;
    }   

}


int main()
{

    ig = 0;
    setInitialStates();

    while (1)
    {

        right_triggered();
        left_triggered();
        rear_triggered();

        selectFrontWheel();  //checks if left or right wheel is faster
        adjustSpeed();   //cuts engine based on difference of wheel speeds


    }
}
