#include <mbed.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ledpin LED3
#define Pi 3.1415

DigitalOut ig(D6);
DigitalOut out(ledpin);

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


double calculateSpeed(double ticks, int micros);


void right_triggered()
{
    out = !out;

    float test = tf1.read();
    if (test > 2.5) {
        rightSpeed = 0.0;
        tf1.reset();
    }

    if (rightCount == 0)
    {
        tf1.start();
        rightCount++;
    }

    else if (rightCount == 5)
    {
        tf1.stop();

        time1 = tf1.read_us();
        rightSpeed = calculateSpeed(5.0, time1);

        rightCount = 0;
        tf1.reset();
    }
    else
    {
        rightCount++;
    }
}

void left_triggered()
{
    out = !out;

    float test = tf2.read();
    if (test > 2.5) {
        leftSpeed = 0.0;
        tf2.reset();
    }

    if (leftCount == 0)
    {
        tf2.start();
        leftCount++;
    }

    else if (leftCount == 5)
    {

        tf2.stop();

        time2 = tf2.read_us();
        leftSpeed = calculateSpeed(5.0, time2);

        leftCount = 0;
        tf2.reset();
    }
    else
    {
        leftCount++;
    }
}

void rear_triggered()
{
    out = !out;

    float test = r.read();
    if (test > 2.5) {
        rearSpeed = 0.0;
        r.reset();
    }

    if (rearCount == 0)
    {
        r.start();
        rearCount++;
    }

    else if (rearCount == 25)
    {

        r.stop();

        time3 = r.read_us();
        rearSpeed = calculateSpeed(25.0, time3);
        //rearAccel = calculateAcceleration(prev, speed, time3);

        rearCount = 0;
        r.reset();
    }
    else
    {
        rearCount++;
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

    while (1)
    {

        right_triggered();
        left_triggered();
        rear_triggered();

        selectFrontWheel();  //checks if left or right wheel is faster
        adjustSpeed();   //cuts engine based on difference of wheel speeds


    }
}
