#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

////////GLOBAL VARIABLES///////////////
#define QTY_ZONES 2 //NOTE: relay and hygrometer count must be equal to zone quantity

//power pin assignments
#define HYGRO_POWER_PIN 2
#define PUMP_POWER_PIN 20
//hygro pin assignments
#define DIGITAL_HYGRO_PIN1 16
#define DIGITAL_HYGRO_PIN2 17
//solenoid pin assignments
#define RELAY1 18
#define RELAY2 19

//array of hygrometers
int hygrometers[QTY_ZONES] = {DIGITAL_HYGRO_PIN1, DIGITAL_HYGRO_PIN2};

//array of relays
int relays[QTY_ZONES] = {RELAY1, RELAY2};

//struct for each garden zone & array created.
struct garden_zone
{
    int hygrometer;
    bool hygro_reading;
    int relay;
    //bool relay_power;
} garden_zones[QTY_ZONES];

////////////////FUNCTION DECLARATIONS///////////////////
void init_pump();
void init_hygros(int hygro_id);
void init_relays(int relay_id);
void init_zones();
void init_system();
void record_hygros();
void run_read_cycle();
void run_water_cycle();

//test functions
//

////////////////MAIN PROGRAM (CORE 0)/////////////////
int main()
{
    stdio_init_all();
    printf("Indoor herb garden v 1.0\n");

    //initialize power, components, and zones
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

/////////////////FUNCTION IMPLEMENTATIONS//////////////
void init_pump()
{
    gpio_init(PUMP_POWER_PIN);
    gpio_set_dir(PUMP_POWER_PIN, GPIO_OUT);
    gpio_put(PUMP_POWER_PIN, 1);
}

void init_hygros(int hygro_id)
{
    gpio_init(hygro_id);
    gpio_set_dir(hygro_id, GPIO_IN);
    gpio_pull_down(hygro_id);
}

void init_relays(int relay_id)
{
    gpio_init(relay_id);
    gpio_set_dir(relay_id, GPIO_OUT);
    gpio_put(relay_id, 1);
}

//set components into their zones
void init_zones()
{

    for (int i = 0; i < QTY_ZONES; ++i)
    {
        garden_zones[i].hygrometer = hygrometers[i];
        init_hygros(garden_zones[i].hygrometer);

        garden_zones[i].relay = relays[i];
        init_relays(garden_zones[i].relay);
    }
}

//need to add lights still
void init_system()
{
    //init power pin for hygros
    gpio_init(HYGRO_POWER_PIN);
    gpio_set_dir(HYGRO_POWER_PIN, GPIO_OUT);

    //init pump power pin
    init_pump();
    //init the zones by setting components to each grow zone
    init_zones();
}

void record_hygros()
{
    for (int i = 0; i < QTY_ZONES; ++i)
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
            //printf("Watering zone %u\n", i);  //serial output test function
        }
        else
        {
            gpio_put(garden_zones[i].relay, 1);
            //printf("Zone %u okay\n", i);  //serial output test function
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