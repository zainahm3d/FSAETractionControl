#include <mbed.h>
#include "PID.h"

#include "nRF24L01.h"
#include "RF24.h"
#include <SPI.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// InterruptIn left(D11);
// InterruptIn right(D12);
// InterruptIn rear(D12);

DigitalOut ig(D6);

 #define ledpin LED3
 #define Pi 3.1415

const double diameter = 1.3333333; // in feet
const double radius = 0.666666;    // in feet
const double circ = diameter * Pi;

double rightSpeed = 0;
double leftSpeed = 0;
double rearSpeed = 0;

double frontSpeed = 0;

// unsigned short delta = 0;
// unsigned char highByte = 0;
// unsigned char lowByte = 0;

DigitalOut out(ledpin);

// #define nrf_CE A1
// #define nrf_CSN A0
// #define spi_SCK A4
// #define spi_MOSI A6
// #define spi_MISO A5

// RF24 radio(spi_MOSI, spi_MISO, spi_SCK, nrf_CE, nrf_CSN);

const uint64_t pipe = 0xE8E8F0F0E1LL;

//CAN can1(D10, D2); // CAN bus pins

Timer tf1;
Timer tf2;
Timer r;
//Timer accelTimer;

long time1 = 1;
long time2 = 1;
long time3 = 1;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

double speed;

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

/*
double calculateAcceleration(double prev, double newSpeed, double t) {

    double t2 = t/1000000.0;

    double accel = (newSpeed - prev)/(t2);

    return accel; 
}
*/

int main()
{

    // radio.begin();
    // radio.openWritingPipe(pipe);
    // radio.setPALevel(RF24_PA_HIGH);
    // radio.setDataRate(RF24_250KBPS);
    // radio.stopListening();

    //right.fall(&right_triggered);
    //left.fall(&left_triggered);
    //rear.fall(&rear_triggered);

 
    //left.mode(PullUp);
    //right.mode(PullUp);
    //rear.mode(PullUp);
 
    ig = 0;

    while (1)
    {

        wait(0.2);
        
        //out = !out;
        // printf("%s","Rear speed: ");
        // printf("%f\n",rearSpeed);

        if (leftSpeed >= rightSpeed)
        {
            frontSpeed = leftSpeed;
        }
        else
        {
            frontSpeed = rightSpeed;
        }
        // frontSpeed = 1.5;
        // rearSpeed = 1.2;

        // printf("%s","Rear speed: ");
        // printf("%f\n",rearSpeed);

        // int f = round(frontSpeed);
        // int r = round(rearSpeed);
        // int n = '\n';

        //radio.write(&frontSpeed, sizeof(frontSpeed));
        //radio.write(&rearSpeed, sizeof(rearSpeed));
        
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

            // wait(.1);
            // printf("%f\n", diff);
        
    }
}
