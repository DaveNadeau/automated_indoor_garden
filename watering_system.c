#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

//global variables
#define QTY_HYGROS 2

//pin assignments
#define HYGRO_POWER_PIN 2
#define DIGITAL_HYGRO_PIN1 16
#define DIGITAL_HYGRO_PIN2 17

int main()
{
    stdio_init_all();
    printf("indoor herb garden v 1.0\n");

    // adc_init();

    // // Make sure GPIO is high-impedance, no pullups etc
    // adc_gpio_init(26);
    // // Select ADC input 0 (GPIO26)
    // adc_select_input(0);

    //intialized digital pins to power and read hygrometer
    gpio_init(HYGRO_POWER_PIN);
    gpio_init(DIGITAL_HYGRO_PIN1);
    gpio_init(DIGITAL_HYGRO_PIN2);

    // set pin to output power
    gpio_set_dir(HYGRO_POWER_PIN, GPIO_OUT);

    //set pin to be read (so as an input) and pull down for 0 reading (not sure if needed) for device 1
    gpio_set_dir(DIGITAL_HYGRO_PIN1, GPIO_IN);
    gpio_pull_down(DIGITAL_HYGRO_PIN1);

    //set pin to be read (so as an input) and pull down for 0 reading (not sure if needed) for device 2
    gpio_set_dir(DIGITAL_HYGRO_PIN2, GPIO_IN);
    gpio_pull_down(DIGITAL_HYGRO_PIN2);

    while (1)
    {
        // // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        // const float conversion_factor = 3.3f / (1 << 12);
        // uint16_t result = adc_read();
        // printf("Raw value: %u, voltage: %f V\n", result, result * conversion_factor);

        //power up the hygrometers and let them stablize
        gpio_put(HYGRO_POWER_PIN, 1);
        sleep_ms(200);

        //digital read 1
        bool digital_result1 = gpio_get(DIGITAL_HYGRO_PIN1);
        if (!digital_result1)
        {
            printf("Unit 1 Digital value: %d (water okay)\n", digital_result1);
        }
        else
        {
            printf("Unit 1 Digital value: %d (needs water)\n", digital_result1);
        }

        //digital read 2
        bool digital_result2 = gpio_get(DIGITAL_HYGRO_PIN2);
        if (!digital_result2)
        {
            printf("Unit 2 Digital value: %d (water okay)\n", digital_result2);
        }
        else
        {
            printf("Unit 2 Digital value: %d (needs water)\n", digital_result2);
        }

        //power down hygro until next reading
        gpio_put(HYGRO_POWER_PIN, 0);

        //based on stored value from hygro, start pump and open required solenoids

        sleep_ms(10000);
    }
}
