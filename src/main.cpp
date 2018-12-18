#include <mbed.h>
#include "PID.h"

// PID stuff 
#define RATE 0.002

//Kc, Ti, Td, interval

// Pid End 

InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D12);
PwmOut ignitionOut(D6);

#define ledpin LED3
#define Pi  3.1415

const double diameter = 1.3333333; // in feet
const double radius = 0.666666; // in feet
const double circ = diameter*Pi;

double rightSpeed = 0;
double leftSpeed = 0;
double rearSpeed = 0;

double frontSpeed = 0;

unsigned short delta = 0;
unsigned char highByte = 0;
unsigned char lowByte = 0;

DigitalOut out(ledpin);

CAN can1(D10, D2); // CAN bus pins

Timer tf1;
Timer tf2;
Timer r;
Timer accelTimer;

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
    }
    else  {
        rightCount++;    
    }
}


void left_triggered()
{

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
    }
    else  {
        leftCount++;    
    }
}


void rear_triggered()
{

    out = !out;

    double prev;

    if (rearCount == 0) {
        r.start();
        rearCount++;
    }

    else if (rearCount == 25) {

        r.stop();
                        
        time3 = r.read_us();
        rearSpeed = calculateSpeed(25.0, time3);
        //rearAccel = calculateAcceleration(prev, speed, time3);


        rearCount = 0;
        r.reset();

        prev = rearSpeed; 
    }
    else  {
        rearCount++;    
    }
}

double calculateSpeed(double ticks, int micros) {

    double revs;
    double speed; 
    double seconds = micros/1000000.0;

    double ticksPerSecond = (ticks/seconds); // ticks per second 

    if (ticks == 5) {
        revs = ticksPerSecond/25.0; //revs per second
    }
    else {
        revs =  ticksPerSecond/100.0;
    }

    speed = revs*2.8553; // 2.8553 is the number that converts rev/s to linear speed 
                        // (wheel size dependent)


    return speed;
    
}

double calculateAcceleration(double prev, double newSpeed, double t) {

    double t2 = t/1000000.0;

    double accel = (newSpeed - prev)/(t2);

    return accel; 
}

int main()
{
    PID controller(0.0, 0.0, 1.0, 0.02);

    out.write(0);

    float pidResult = 0.0;

    left.mode(PullUp);
    right.mode(PullUp);
    rear.mode(PullUp);

    right.rise(&right_triggered);
    left.rise(&left_triggered);
    rear.fall(&rear_triggered);

    controller.setInputLimits(-30.0, 30.0);
    controller.setOutputLimits(0.0, 1.0);
    controller.setMode(0); //manual mode
    //controller.setTunings(1.0,1.0,1.0);
    //We want difference to be 0 mph
    controller.setSetPoint(15.0);

    
    while (1) {
       
        // printf("%f\n",rightSpeed);
        // printf("%f\n",leftSpeed);
        printf("%s","Rear speed: ");
        printf("%f\n",rearSpeed);


        if (leftSpeed >= rightSpeed) {
            frontSpeed = leftSpeed;
        }
        else {
            frontSpeed = rightSpeed;
        }
        printf("%s","Front speed: ");
        printf("%f\n\n",frontSpeed);

        /*
        delta = (frontSpeed / rearSpeed) *100;
        printf("%f\n",delta);
        highByte = (delta >> 8) & 0xFF;
        lowByte = delta & 0xFF;
        */
       float diff = rearSpeed - frontSpeed;
       diff = abs(diff);

       printf("%f\n",diff);
       controller.setProcessValue(diff);
       pidResult = controller.compute();

       printf("%f\n\n", pidResult);
       ignitionOut.write(pidResult);


    }   
}