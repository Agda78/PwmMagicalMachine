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
#include <cstdint>

#define PwmPin PC_7
#define FrUpdate 200ms
#define MainSleep 5000ms
#define ShellDelay 30ms

typedef struct{
        uint32_t period; //Period in [us]
        float duty_cicle;   //Duty cicle in [%]
    }pwm_manager;

static BufferedSerial serial(USBTX, USBRX); //Set serial comunication
PwmOut pinPwm(PwmPin);

Thread shellThread;
Thread pwmManagerThread;

static pwm_manager led;

void shellThd(){ //Method for managment of a shell
    //Manager of shell 
        int scelta = 0;
        char menu[] = "\n--------[MENU]--------\n1.Set Period [us]\n2.Set Duty Cicle [%]\n3.Print Current Status\nChoice: ";
        
        char periodMessage[] = "\n--------[OPTIONS]--------\n+) Period + 1 [us]\n-) Period - 1 [us]\n0) Period = 1 [us]\n1) Period + 2 [us]\n2) Period + 5 [us]\n3) Period + 10 [us]\n4) Period + 100 [us]\n5) Period + 1000 [us]\n6) Period + 10 [ms]\n7) Period + 100 [ms]\n8) Period + 1000 [ms]\nD) Period = 10 [us]\nC) Period = 100 [us]\nM) Period = 1000 [us]\nE) EXIT FROM PERIOD SETTING\nValue: ";

        char dutyMessage[] = "\n--------[OPTIONS]--------\n+) Duty + 1 [%]\n-) Duty - 1 [%]\n0) Duty = 1 [%]\n1) Duty + 2 [%]\n2) Duty + 5 [%]\n3) Duty + 10 [%]\n4) Duty + 50 [%]\nD) Duty = 10 [%]\nC) Duty = 50 [%]\nM) Duty = 100 [%]\nE) EXIT FROM DUTY SETTING\nValue: ";

        char accapo[] = "\n";


        char errore[] = "No option in the list\n";

        char *buff = new char[1];
        int percent;

        while(1){
            
            serial.write(menu, sizeof(menu));
            serial.read(buff, sizeof(buff));
            serial.write(buff, sizeof(buff));
            serial.write(accapo, sizeof(accapo));

            switch(*buff){
                case '1':
                    printf("\n[CURRENT] Period value: %d\n", led.period);
                    do{
                        serial.write(periodMessage, sizeof(periodMessage));
                        serial.read(buff, sizeof(buff));
                        serial.write(buff, sizeof(buff));
                        switch (*buff) {
                            case '+':
                                led.period++;
                                break;
                            case '-':
                                if(led.period > 1){
                                    led.period--;
                                }else{
                                    printf("\nMinimum value\n");
                                }
                                break;
                            case '0':
                                led.period = 1;
                                break;

                            case '1':
                                led.period += 2;
                                break;
                                
                            case '2':
                                led.period += 5;
                                break;
                            
                            case '3':
                                led.period += 10;
                                break;
                            
                            case '4':
                                led.period += 100;
                                break;
                            case '5':
                                led.period += 1000;
                                break;
                            case '6':
                                led.period += 10000;
                                break;
                            case '7':
                                led.period += 100000;
                                break;
                            case '8':
                                led.period += 1000000;
                                break;
                            case 'D':
                                led.period = 10;
                                break;
                            
                            case 'C':
                                led.period = 100;
                                break;
                            
                            case 'M':
                                led.period = 1000;
                                break;
                            
                            case 'E':
                                printf("\n[UPDATE] Period: %d\n", led.period);
                                break;

                            default:
                                printf("\nValue not recognized\nRetry and insert a value in [0,9] or + and - [follow the menu]\n");
                            }
                            if(*buff != 'E'){
                                printf("\n[UPDATED] Period value: %d\n", led.period);
                            }
                        }while(*buff != 'E');
                        break;
                    
                case '2':
                    percent = (int)(led.duty_cicle * 100.0f);
                    printf("\n[CURRENT] Duty value: %d\n", percent);
                    do{
                        serial.write(dutyMessage, sizeof(dutyMessage));
                        serial.read(buff, sizeof(buff));
                        serial.write(buff, sizeof(buff));
                        switch (*buff) {
                            case '+':
                                percent++;
                                break;
                            case '-':
                                if(led.period > 0){
                                    percent--;
                                }else{
                                    printf("\nMinimum value\n");
                                }
                                break;
                            case '0':
                                percent = 1;
                                break;

                            case '1':
                                percent += 2;
                                break;
                                
                            case '2':
                                percent += 5;
                                break;
                            
                            case '3':
                                percent += 10;
                                break;
                            
                            case '4':
                                percent += 50;
                                break;
                            
                            case 'D':
                                percent = 10;
                                break;
                            
                            case 'C':
                                percent = 50;
                                break;
                            
                            case 'M':
                                percent = 100;
                                break;
                            
                            case 'E':
                                printf("\n[UPDATE] Duty: %d\n", percent);
                                break;

                            default:
                                printf("\nValue not recognized\nRetry and insert a value in [0,9] or + and - [follow the menu]\n");
                            }
                            
                            led.duty_cicle = (float)((float)(percent)/100.0f);

                            if(*buff != 'E'){
                                printf("\n[UPDATED] Duty value: %d\n", percent);
                            }

                        }while(*buff != 'E');
                
                    break;

                case '3':
                    printf("\n--------[INFO PWM]--------\n");
                    if(led.period < 100000){
                        printf("Period: %d [us]\n", led.period);
                    }else{
                            printf("Period: %d [ms]\n",(int) (led.period/1000));
                    }
                    printf("Duty Cicle: %d [%%]\n\n", (int)(led.duty_cicle * 100.0f));
                    break;

                default:
                    serial.write(errore, sizeof(errore));  
            }

        }
        ThisThread::sleep_for(ShellDelay);
} 

void pwmManagerThd(){ //Method for control and set the pwm parameters
    //Manager of pwm
    float previusDutyCicle = 0.0f;
    while(1){
        if(pinPwm.read_period_us() != led.period){
            pinPwm.period_us(led.period);
        }
        if(led.duty_cicle != previusDutyCicle){
            previusDutyCicle = led.duty_cicle;
            pinPwm.write(led.duty_cicle);
        }
        ThisThread::sleep_for(FrUpdate);
    }
} 


int main()
{
    shellThread.start(shellThd);
    pwmManagerThread.start(pwmManagerThd);

    led.period = 10;    //10 us
    led.duty_cicle = 0.5f; //50 %
     while(1){

         ThisThread::sleep_for(MainSleep);
     }    
}


