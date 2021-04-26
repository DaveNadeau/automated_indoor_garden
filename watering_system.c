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
#define PUMP_POWER_PIN 20
#define DIGITAL_HYGRO_PIN1 16
#define DIGITAL_HYGRO_PIN2 17

#define RELAY1 18
#define RELAY2 19

//array of hygrometers
int hygrometers[QTY_HYGROS] = {DIGITAL_HYGRO_PIN1, DIGITAL_HYGRO_PIN2};

//array of relays
int relays[QTY_RELAYS] = {RELAY1, RELAY2};

//struct for each garden zone & array created. currently only using hygro_reading
//but we could assign the arrays and relays into this and run everything from the struct
struct garden_zone
{
    int hygrometer;
    bool hygro_reading;
    int relay;
    //bool relay_power;
} garden_zones[QTY_ZONES];

//function declarations
void init_hygros();
void init_pump();
void init_relays();
void init_zones();
void init_system();
void record_hygros();
void run_read_cycle();
void run_water_cycle();

//test functions
//

int main()
{
    stdio_init_all();
    printf("Indoor herb garden v 1.0\n");

    //initialize components
    init_system();

    while (1)
    {
        //read hygrometers
        run_read_cycle();
        //water if necessary
        run_water_cycle();
        //sleep until next reading
        sleep_ms(10000);
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

void init_pump()
{
    gpio_init(PUMP_POWER_PIN);
    gpio_set_dir(PUMP_POWER_PIN, GPIO_OUT);
    gpio_put(PUMP_POWER_PIN, 1);
}

void init_relays()
{
    for (int i = 0; i < QTY_RELAYS; ++i)
    {
        gpio_init(relays[i]);
        gpio_set_dir(relays[i], GPIO_OUT);
        gpio_put(relays[i], 1);
    }
}

//set components into their zones
void init_zones()
{
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        garden_zones[i].hygrometer = hygrometers[i];
        garden_zones[i].relay = relays[i];
    }
}

//need to add lights still
void init_system()
{
    init_hygros();
    init_pump();
    init_relays();
    init_zones();
}

void record_hygros()
{
    for (int i = 0; i < QTY_HYGROS; ++i)
    {
        bool digital_reading = gpio_get(garden_zones[i].hygrometer);
        garden_zones[i].hygro_reading = digital_reading;
    }
}

void run_read_cycle()
{
    //power up the hygrometers and let them stablize
    gpio_put(HYGRO_POWER_PIN, 1);
    sleep_ms(200);

    //read and save hygro values into garden zone struct
    record_hygros();

    //power down hygro until next reading
    gpio_put(HYGRO_POWER_PIN, 0);
}

void run_water_cycle()
{
    //Open solenoids of the zones that need water only
    bool needs_water = false;
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        if (garden_zones[i].hygro_reading)
        {
            gpio_put(garden_zones[i].relay, 0);
            needs_water = true;
            printf("Watering zone %u\n", i);
        }
        else
        {
            gpio_put(garden_zones[i].relay, 1);
            printf("Zone %u okay\n", i);
        }
    }

    // if one of the readings says it needs water, then turn on the pump
    // if (needs_water)
    // {
    //     gpio_put(PUMP_POWER_PIN, 0);
    // }

    //let the water run for a few seconds
    sleep_ms(4000);

    // turn off pump power and close all relays
    gpio_put(PUMP_POWER_PIN, 1);

    for (int i = 0; i < QTY_ZONES; ++i)
    {
        gpio_put(relays[i], 1);
    }
}

//test function implementations
//