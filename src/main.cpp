#include <mbed.h>

InterruptIn front1(D3);
InterruptIn front2(D4);
InterruptIn rear(D5);
PwmOut sparkCut(D6);

Timer tf1;
Timer tf2;
Timer r;

bool states[3];

int f1[6];
void front1_triggered()
{
    tf1.start();
    int i = 0;

    if (states[0] == true)
    {
        f1[i] = tf1.read_us();
        i++;
    }

    states[0] = !states[0];
}

int f2[6];
void front2_triggered()
{
    tf1.start();
    states[1] = !states[1];
}

int r1[6];
void rear_triggered()
{
    tf1.start();
    states[2] = !states[2];
}

int main()
{
    front1.rise(&front1_triggered);
    front2.rise(&front2_triggered);
    rear.rise(&rear_triggered);

    for (int i = 0; i < 3; i++)
    {
        states[i] = false;
    }

    int x;
    
    while (1)
    {
    }
}