#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
using namespace std::chrono;

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);

BufferedSerial pc(USBTX, USBRX);
BufferedSerial xbee(D1, D0);

//BufferedSerial pc(D1, D0);
//BufferedSerial xbee(USBTX, USBRX);

DigitalInOut ping(D11);
parallax_ping ping1(ping);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

BBCar car(pin5, pin6, servo_ticker);

int state = 0;
int rest = 0;
int turnNext = 0;
int finish = 0;
int nextTask = 0;
int curTask = 0;
int step = 0;
int point[3] = {0};
int start = 0;
int lock1 = 0, lock2 = 0;

void parking(Arguments *in, Reply *out);
RPCFunction rpcparking(&parking, "parking");

void detectItem(float storeDistance1, float storeDistance2, float storeDistance3) {
    while(1) {
        
        car.stop();
        ThisThread::sleep_for(1000ms);

        car.turn(20, 0.1); // turn left
        ThisThread::sleep_for(2200ms);
        storeDistance1 = (float)ping1;
        printf("distance1: %.1f\r\n", storeDistance1);
        
        car.goStraight(-60);
        ThisThread::sleep_for(500ms);
        car.turn(20, -0.1); // turn right
        ThisThread::sleep_for(1300ms);
        car.stop();
        ThisThread::sleep_for(2000ms);
        storeDistance2 = (float)ping1;
        printf("distance2: %.1f\r\n", storeDistance2);

        car.turn(20, -0.1);
        ThisThread::sleep_for(1600ms);
        storeDistance3 = (float)ping1;
        printf("distance3: %.1f\r\n", storeDistance3);

        if (storeDistance1 > storeDistance2 && lock1 == 0) {
            if (storeDistance2 > storeDistance3) {
                nextTask = 2;
            }
            else {
                nextTask = 3;
            }
        } else if (storeDistance1 < storeDistance2) {
            if (storeDistance2 < storeDistance3) {
                nextTask = 4;
            }
            else {
                nextTask = 3;
            }
        } 

        car.stop();
        break;
    }
}

void parking(Arguments *in, Reply *out) {

    start = in->getArg<int>();
    
    char buf[10] = {'\0'};
    char o = '0';
    buf[0] = '0';
    //xbee.write(&o, 2);
    //xbee.write(buf, sizeof(buf));
    xbee.write("OK\n", 4);
    

    while(start) {

        float distance = 0;
        float storeDistance1 = 0, storeDistance2 = 0, storeDistance3 = 0;
        distance = (float)ping1;
        printf("current distance: %.1f\r\n", distance);

        if (state == 0) {
            car.goStraight(50);
            ThisThread::sleep_for(20ms);

            if (distance < 15) {
                detectItem(storeDistance1, storeDistance2, storeDistance3);
                rest = 1;
            }  
        } 
        else if (state == 1) {
            car.goStraight(50);
            ThisThread::sleep_for(20ms);

            if (distance < 15) {
                detectItem(storeDistance1, storeDistance2, storeDistance3);
                rest = 1;
            } 

            /*nextTask = -1;
            rest = 1;*/
        }
        else if (state == 2) {
            car.goStraight(50);
            ThisThread::sleep_for(20ms);
            
            if (distance < 15) {
                detectItem(storeDistance1, storeDistance2, storeDistance3);
                rest = 1;
            }

            /*nextTask = -1;
            rest = 1;*/
        } 
        else if (state == 3) {
            car.goStraight(-100);
            ThisThread::sleep_for(1500ms);

            float stopDistance = (float)ping1;

            if (stopDistance < 5) {
                nextTask = -1;
                rest = 1;
            }
            
            //if (distance < 10) {
            nextTask = -1;
            rest = 1;
            //}
        } 

        if (rest == 1) {
            if (nextTask == 2) {
                state = 1;
                lock1 = 1;
                printf("It is task %d\r\n", state);
                printf("\r\n");

                car.goStraight(-50);
                ThisThread::sleep_for(1800ms);
                //car.turn(40, 0.2); //turn left
                //ThisThread::sleep_for(4200ms); //4000
                car.turn(80, 0.2); //turn left
                ThisThread::sleep_for(2400ms); //4000
                car.stop();
                ThisThread::sleep_for(1500ms);
            }
            else if (nextTask == 3) {
                state = 2;
                printf("It is task %d\r\n", state);
                printf("\r\n");

                car.goStraight(-50);
                ThisThread::sleep_for(1400ms);
                //car.turn(40, -0.1); //turn right
                //ThisThread::sleep_for(1500ms);
                car.turn(80, -0.1); //turn right
                ThisThread::sleep_for(800ms);
                car.stop();
                ThisThread::sleep_for(1500ms);
            }
            else if (nextTask == 4) {
                state = 3;
                printf("It is task %d\r\n", state);

                car.goStraight(-50);
                ThisThread::sleep_for(1800ms);
                /*car.turn(40, 0.2); //turn left
                ThisThread::sleep_for(3800ms);*/
                car.turn(80, -0.1); //turn right
                ThisThread::sleep_for(1150ms);
                car.stop();
                ThisThread::sleep_for(1500ms);
            } 
            else if (nextTask == -1) {
                printf("Finish Mission\r\n");
                car.stop();
                start = 0;
                break;
            }
            rest = 0;
        }

    }

}

/*void distanceMeasure() {
    float distance = 0;
    while(1) {
        distance = (float)ping1;
        if(state == 1) {
            printf("distance1: %.1f\r\n", distance);
            state = 0;
        } else if(state == 2) {
            printf("distance2: %.1f\r\n", distance);
            state = 0;
        } else if(state == 3) {
            printf("distance3: %.1f\r\n", distance);
            state = 0;
        }
    }
}*/

int main() {
    
    //t.start(callback(&queue, &EventQueue::dispatch_forever));
    //pc.set_baud(9600);
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&xbee, "r");
    FILE *devout = fdopen(&xbee, "w");
    while (1) {
        memset(buf, 0, 256);
        for( int i = 0; ; i++ ) {
            char recv = fgetc(devin);
            if(recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
    RPC::call(buf, outbuf);
    }
    //queue.call(distanceMeasure);

    
}