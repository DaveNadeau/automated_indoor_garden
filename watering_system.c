#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/////////////////GLOBAL VARIABLES//////////////////
#define QTY_ZONES 3 //NOTE: relay and hygrometer count must be equal to zone quantity

/////PIN ASSIGNMENTS/////
//Hygrometer power pin assignments
#define HYGRO_POWER1 19
#define HYGRO_POWER2 20
#define HYGRO_POWER3 21
//hygro pin assignments
#define DIGITAL_HYGRO_PIN1 16
#define DIGITAL_HYGRO_PIN2 17
#define DIGITAL_HYGRO_PIN3 18
//solenoid pin assignments
#define SOLENOID1 15
#define SOLENOID2 14
#define SOLENOID3 13
//Light and pump pin assignments
#define PUMP_RELAY_PIN 12
#define LIGHT_RELAY_PIN 11

//timing variables for reading and watering
#define MAIN_SLEEP 10000
#define HYGRO_READ_DELAY 500
#define WATERING_DELAY 4000

//timing variables for lighting
#define LIGHT_ON_DELAY 20000
#define LIGHT_OFF_DELAY 5000

//arrays of hygrometer's read and power pins
int hygrometers[QTY_ZONES] = {DIGITAL_HYGRO_PIN1, DIGITAL_HYGRO_PIN2, DIGITAL_HYGRO_PIN3};
int hygro_power[QTY_ZONES] = {HYGRO_POWER1, HYGRO_POWER2, HYGRO_POWER3};

//array of solenoids
int solenoids[QTY_ZONES] = {SOLENOID1, SOLENOID2, SOLENOID3};

//struct for each garden zone & array of zones created.
struct garden_zone
{
    int hygrometer;
    int hygro_power;
    bool hygro_reading;
    int solenoid;
} garden_zones[QTY_ZONES];

////////////////FUNCTION DECLARATIONS///////////////////
void init_pump();
void init_light();
void init_hygro_power(int hygro_power_id);
void init_hygros(int hygro_id);
void init_solenoids(int relay_id);
void init_zones();
void init_system();
void record_hygros();
void run_read_cycle();
void run_water_cycle();
void run_light_cycle();

////////////////MAIN PROGRAM (CORE 0)/////////////////
int main()
{
    stdio_init_all();
    printf("Indoor herb garden v 1.0\n");

    //use second core for light timing/function
    multicore_launch_core1(run_light_cycle);

    //initialize power, components, and zones
    init_system();
    //main loop
    while (1)
    {
        //read hygrometers
        run_read_cycle();
        //water if necessary
        run_water_cycle();
        //sleep until next reading
        sleep_ms(MAIN_SLEEP);
    }
}

/////////////////FUNCTION IMPLEMENTATIONS//////////////
void init_pump()
{
    gpio_init(PUMP_RELAY_PIN);
    gpio_set_dir(PUMP_RELAY_PIN, GPIO_OUT);
    gpio_put(PUMP_RELAY_PIN, 1);
}

void init_light()
{
    gpio_init(LIGHT_RELAY_PIN);
    gpio_set_dir(LIGHT_RELAY_PIN, GPIO_OUT);
    gpio_put(LIGHT_RELAY_PIN, 1);
}

void init_hygro_power(int hygro_power_id)
{
    gpio_init(hygro_power_id);
    gpio_set_dir(hygro_power_id, GPIO_OUT);
}

void init_hygros(int hygro_id)
{
    gpio_init(hygro_id);
    gpio_set_dir(hygro_id, GPIO_IN);
    gpio_pull_down(hygro_id);
}

void init_solenoids(int relay_id)
{
    gpio_init(relay_id);
    gpio_set_dir(relay_id, GPIO_OUT);
    gpio_put(relay_id, 1);
}

//set components into their zones and initialze their pins
void init_zones()
{

    for (int i = 0; i < QTY_ZONES; ++i)
    {
        //set hygrometers to zones and initialize their pins
        garden_zones[i].hygrometer = hygrometers[i];
        garden_zones[i].hygro_power = hygro_power[i];
        init_hygros(garden_zones[i].hygrometer);
        init_hygro_power(garden_zones[i].hygro_power);
        //set solenoids to zones and initialize their pins
        garden_zones[i].solenoid = solenoids[i];
        init_solenoids(garden_zones[i].solenoid);
    }
}

//need to add lights still
void init_system()
{
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
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        gpio_put(garden_zones[i].hygro_power, 1);
    }
    sleep_ms(HYGRO_READ_DELAY);
    //read and save hygro values into garden zone struct
    record_hygros();
    //power down hygro until next reading
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        gpio_put(garden_zones[i].hygro_power, 0);
    }
}

void run_water_cycle()
{
    //Open solenoids of the zones that need water only
    bool needs_water = false;
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        if (garden_zones[i].hygro_reading)
        {
            gpio_put(garden_zones[i].solenoid, 0);
            needs_water = true;
            //printf("Watering zone %u\n", i);  //serial output test function
        }
        else
        {
            gpio_put(garden_zones[i].solenoid, 1);
            //printf("Zone %u okay\n", i);  //serial output test function
        }
    }

    // if one of the readings says it needs water, then turn on the pump
    if (needs_water)
    {
        //start pump
        gpio_put(PUMP_RELAY_PIN, 0);
        //let the water run for a few seconds
        sleep_ms(WATERING_DELAY);
        // turn off pump
        gpio_put(PUMP_RELAY_PIN, 1);
    }

    // close all solenoids
    for (int i = 0; i < QTY_ZONES; ++i)
    {
        gpio_put(solenoids[i], 1);
    }
}

void run_light_cycle()
{
    //init light relay pin
    init_light();
    while (1)
    {
        gpio_put(LIGHT_RELAY_PIN, 0);
        sleep_ms(LIGHT_ON_DELAY);
        gpio_put(LIGHT_RELAY_PIN, 1);
        sleep_ms(LIGHT_OFF_DELAY);
    }
}

//test function implementations
