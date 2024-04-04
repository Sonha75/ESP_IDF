#ifndef LED_C_H
#define LED_C_H

void LEDC_INIT(int pin, int channel);
void LEDC_SET_DUTY(int channel, int duty);


#endif