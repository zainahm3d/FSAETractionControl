#include <mbed.h>


InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D5);
PwmOut sparkCut(D6);

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

    if (rearCount == 0) {
        r.start();
        rearCount++;
    }

    else if (rearCount == 25) {

        r.stop();
                        
        time3 = r.read_us();
        rearSpeed = calculateSpeed(25.0, time3);

        rearCount = 0;
        r.reset();
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

int main()
{

    out.write(0);

    left.mode(PullUp);
    right.mode(PullUp);
    rear.mode(PullUp);

    right.rise(&right_triggered);
    left.rise(&left_triggered);
    rear.rise(&rear_triggered);

    while (1) {
       

        // wait(1);
        // printf("%f\n",rightSpeed);
        // printf("%f\n",leftSpeed);
        // printf("%f\n\n",rearSpeed);

        if (leftSpeed >= rightSpeed) {
            frontSpeed = leftSpeed;
        }
        else {
            frontSpeed = rightSpeed;
        }

        delta = (frontSpeed / rearSpeed) *100;
        printf("%f\n",delta);
        highByte = (delta >> 8) & 0xFF;
        lowByte = delta & 0xFF;


        can1.frequency(250000);
        can1.mode(CAN::Normal);

        wait(1);
        CANMessage msg;
        // if (can1.read(msg)){
        //     printf("\n%d", msg.id);
        //     printf("%s", " ");
        //     for (int i = 0; i < msg.len; i++){
        //         printf("%d ", msg.data[i]);
        //     }
        //      printf("\n");
        // }
        msg.id = 1;
        msg.data[0] = round(rightSpeed);
        msg.data[1] = round(leftSpeed);
        msg.data[2] = round(rearSpeed);
        msg.data[3] = round (lowByte);
        msg.data[4] = round (highByte);

        can1.write(msg);

    }   
}