#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

//global variables
#define QTY_HYGROS 2
#define QTY_RELAYS 2
#define QTY_ZONES 2

//pin assignments
#define HYGRO_POWER_PIN 2
#define DIGITAL_HYGRO_PIN1 16
#define DIGITAL_HYGRO_PIN2 17

#define RELAY1 18
#define RELAY2 19

//array of hygrometers
int hygrometers[QTY_HYGROS] = {DIGITAL_HYGRO_PIN1, DIGITAL_HYGRO_PIN2};

//array of relays
int relays[QTY_RELAYS] = {RELAY1, RELAY2};

//struct for each garden zone & array created
struct garden_zone
{
    bool hygro_reading;
    bool relay_power;
} garden_zones[QTY_ZONES];

//function declarations
void init_hygros();
void init_relays();
void record_hygros();
void run_water_cycle();

//test functions
void test_relay();

int main()
{
    stdio_init_all();
    printf("Indoor herb garden v 1.0\n");

    //initialize components
    init_hygros();
    init_relays();

    while (1)
    {
        //power up the hygrometers and let them stablize
        gpio_put(HYGRO_POWER_PIN, 1);
        sleep_ms(200);

        //read and save hygro values into garden zone struct
        record_hygros();

        //power down hygro until next reading
        gpio_put(HYGRO_POWER_PIN, 0);

        //water if necessary
        run_water_cycle();

        //sleep until next reading
        sleep_ms(15000);
    }
}

void init_hygros()
{
    //init power pin
    gpio_init(HYGRO_POWER_PIN);
    gpio_set_dir(HYGRO_POWER_PIN, GPIO_OUT);

    //init digital read pins on hygros
    for (int i = 0; i < QTY_HYGROS; ++i)
    {
        gpio_init(hygrometers[i]);
        gpio_set_dir(hygrometers[i], GPIO_IN);
        gpio_pull_down(hygrometers[i]);
    }
}

void init_relays()
{
    for (int i = 0; i < QTY_RELAYS; ++i)
    {
        gpio_init(relays[i]);
        gpio_set_dir(relays[i], GPIO_OUT);
        gpio_put(relays[i], 0);
    }
}

void record_hygros()
{
    for (int i = 0; i < QTY_HYGROS; ++i)
    {
        bool digital_reading = gpio_get(hygrometers[i]);
        garden_zones[i].hygro_reading = digital_reading;
    }
}

void run_water_cycle()
{
    bool needs_water = false;
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        if (garden_zones[i].hygro_reading)
        {
            gpio_put(relays[i], 1);
            needs_water = true;
            printf("Watering zone %u\n", i);
        }
        else
        {
            gpio_put(relays[i], 0);
            printf("Zone %u okay\n", i);
        }
    }

    // if one of the readings says it needs water, then turn on the pump
    // if(needs_water){

    // }
    sleep_ms(7000);

    // turn off power and close relays
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        gpio_put(relays[i], 0);
    }
}

//test function implementations
// void test_relay()
// {
//     //testing relay
//     gpio_init(RELAY1);
//     gpio_set_dir(RELAY1, GPIO_OUT);
//     gpio_put(RELAY1, 0);
//     sleep_ms(2000);
//     gpio_put(RELAY1, 1);
// }

// void read_hygros()
// {
//     //test function... should set zone value for processing later.
//     for (int i = 0; i < QTY_HYGROS; ++i)
//     {
//         bool digital_reading = gpio_get(hygrometers[i]);
//         if (!digital_reading)
//         {
//             printf("Unit %u Digital value: %d (water okay)\n", i, digital_reading);
//         }
//         else
//         {
//             printf("Unit %u Digital value: %d (needs water)\n", i, digital_reading);
//         }
//     }
// }

// void set_garden_zones()
// {
//     for (int i = 0; i < QTY_ZONES; ++i)
//     {
//         garden_zones[i].hygro_reading = hygrometers[i];
//     }
//     for (int i = 0; i < QTY_ZONES; i++)
//     {
//         if (garden_zones[i].hygro_reading)
//         {
//         }
//     }
// }