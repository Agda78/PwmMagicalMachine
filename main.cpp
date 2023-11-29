/*
*   Application for the managment of a PWM period
*   and Duty cicle
*   Author: D'Amora Agostino
*   Github: Agda78
*   Link: https://github.com/Agda78
*   Testing Board: Nucleo64 - G474RE
*   Builder of board: ST Microelectronics
*/

#include "mbed.h"

/*          CONSTANT AREA           */
//Declaration of PINOUT configuration

#define PWMLEDPIN  PC_7    //Pin of pwm
#define ANALOGINPIN PA_0    //Pin of potentiometer
#define BUTTONPIN   PB_5    //Pin D4 for digital Input

//Declaration of MaxPeriod for the management of it
//(We can set all the period that we want)

#define MAXPERIOD   10  //Max period of pwm in [us]
#define MINPERIOD   1   //Min period of pwm in [us]
#define NUMDIVISIONI 10 //Number of division in the choiced range (Choice a NUMDIVISIONI that give a integer number at the division [listen ButtonThread])

//Frequency of operztions (Thread in this case)
#define FRAGGIORNAMENTO 200    //Frequency for the update of pwm data
#define FRBUTTON    100         //Frequency for reduce noice effect
#define FRDISPLAY   1500        //Print Frequency of generic data


/*           MANAGMENT AREA           */
//Declaration for the pwm managment struct
typedef struct{
    int period;
    float duty_cicle;
}pwm_manager;

static pwm_manager manager; //Followed variable for managment of pwm parameter

/*          THREAD AREA           */
//Thread Function for the managment of a button
void buttonThread(){
    //This function manages a period update button

    DigitalIn button(BUTTONPIN);
    int delta = (MAXPERIOD - MINPERIOD + 1)/NUMDIVISIONI;
    int counter = MAXPERIOD - MINPERIOD + 1;

    while(1){
        if(button.read() == 1){
            
            counter = (counter + 1)%NUMDIVISIONI;

            manager.period = MINPERIOD + (counter * delta);

            //printf("[UPDATE] Updated period at: %d\n", manager.period); //Print for eventually debugging

            while(button == 1){}
        }
        thread_sleep_for(FRBUTTON);
    }
}

void displayValuesThread(){
    //Function for the printing of operation
    while(1){
        printf("[CheckSum] Period: %d [us]\tDuty Cicle: %d [%%]\n", manager.period, (int) (manager.duty_cicle*100.0f));
        thread_sleep_for(FRDISPLAY);
    }
}

Thread buttonThd;
Thread displayValuesThd;

int main()
{
    //INIZIALIZZATION OF FOLLOWED VARIABLE
    manager.period = MAXPERIOD;
    manager.duty_cicle = 0.5f;

    //STARTING OF THREADS
    buttonThd.start(buttonThread);  //Button Thread
    displayValuesThd.start(displayValuesThread);    //Display Thread

    //PERIFERAS DECLARATION AREA
    PwmOut pwmLed(PWMLEDPIN);  //Led Pwm
    AnalogIn poten(ANALOGINPIN);    //Potentiometer for dutycicle regulation

    while (true) {
        if ((int)((poten.read()+ 0.01) * 100.0f) != (int) (manager.duty_cicle * 100.0f)) {
            manager.duty_cicle = poten.read() + 0.01;
            pwmLed.write(manager.duty_cicle);
            //printf("[UPDATE] Updating Duty Cicle: %d [%%] \n", (int) (manager.duty_cicle * 100.0f)); //Eventually debugging print
        }
        if (pwmLed.read_period_us() != manager.period) {
            pwmLed.period_us(manager.period);
            //printf("[UPDATE] Updating Period: %d [us]\n", pwmLed.read_period_us()); //Eventually debugging print
        }

        thread_sleep_for(FRAGGIORNAMENTO);
    }
}


