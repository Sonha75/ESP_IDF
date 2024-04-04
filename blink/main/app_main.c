/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "output.h"
#include "input.h"

#define LED 2
#define BIN 0

void HamGoiCallBack(int pin)
{
if(pin==BIN)
{
   static int x=0;
   gpio_set_level(LED,1-x);
   x=1-x;
}
}
void app_main()
{


   output_io_create(LED);

   while(1)
   {
    vTaskDelay(1000/portTICK_PERIOD_MS);
    output_io_toggle(LED);
   }
   // input_io_create(BIN,HI_TO_LO);
   // input_set_callback(HamGoiCallBack);

}
