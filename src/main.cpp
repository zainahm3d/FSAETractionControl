#include <mbed.h>


InterruptIn left(D3);
InterruptIn right(D4);
InterruptIn rear(D5);
PwmOut sparkCut(D6);

#define ledpin LED3
#define Pi  3.14//15926535897

const double diameter = 1.3333333; // in feet
const double radius = 0.875; // in feet
const double circ = diameter*Pi;

DigitalOut out(ledpin);

Timer tf1;
Timer tf2;
Timer r;

long time1 = 0;
long time2 = 0;
long time3 = 0;

int rightCount = 0;
int leftCount = 0;
int rearCount = 0;

bool flag2 = false;


void calculateSpeed(double ticks, int micros);
 

void right_triggered()
{
    rightCount++;
    out.write(!out.read());

    if (rightCount == 0) {

        tf1.start();       
    }

    if (rightCount == 5) {

        tf1.stop();
        time1 = r.read_us();

        printf("%s","Time1 is ");
        printf("%f\n", time1);
        calculateSpeed(5.0, time1);

        rightCount = 0;
        tf1.reset();
    }
}

void left_triggered()
{

    out.write(!out.read());

    if (leftCount == 0) {
        
        printf("%s","Count is: ");
        printf("%d\n",leftCount);
        
        tf2.start();
        printf("%s\n","Started");
        leftCount++;
        flag2 = true;
    }

    if ((leftCount > 0) && (leftCount < 5) && !flag2) {
        
        printf("%s","Count is: ");
        printf("%d\n",leftCount);

        leftCount++;
        
    }

    if (leftCount == 5) {

        tf2.stop();

        printf("%s","Count is: ");
        printf("%d\n",leftCount);
        
                        
        time2 = tf2.read_us();

        printf("%s","Time2 is ");
        printf("%ld\n",time2);
        calculateSpeed(5.0, time2);

        leftCount = 0;
        tf2.reset();
        
    }
    flag2 = false;
}

void rear_triggered()
{
    rearCount++;
    out.write(!out.read());

    if (rearCount == 0) {

        r.start();       
    }
    if (rearCount == 20) {

        r.stop();
        time3 = r.read_us();

        printf("%s","Time3 is ");
        printf("%f\n", time3);
        calculateSpeed(20.0, time3);

        rearCount = 0;
        r.reset();
    }

}

void calculateSpeed(double ticks, int micros) {

    double speed; 
    double seconds = micros/1000000.0;

    
    double ticksPerSecond = (ticks/seconds); // ticks per second 

    double revs = ticksPerSecond/5.0; //revs per second

    double angular = revs * 2.0 * Pi;  // angular speed

    speed = angular*radius; // feet per second

    double mph = speed/(1.467);
   
    printf("%s","mph is ");
    printf("%f\n",mph);
    
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

    
}