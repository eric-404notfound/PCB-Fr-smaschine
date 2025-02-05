#pragma once

#include "Stepper.h"
#include <optional>

#define STEPS_PER_MM 100
#define MAX_DISTANCE (float)0x7FFF / (float)STEPS_PER_MM / (float)MICROSTEPS
#define MAX_AXIS_SPEED 800.0


class Axis
{

private:

    float position;
    
    bool referenced = false;

    double total_moved = 0;

    Stepper stepper;

    char axis_name;

    uint8_t step_pin;
    uint8_t dir_pin;
    
    uint8_t endstop_pin_plus;
    uint8_t endstop_pin_minus;

private:

public:

    const float axisLength;

public:

    Axis(float length, uint8_t step_pin, uint8_t dir_pin, char axis, uint8_t endstop_pin_plus, uint8_t endstop_pin_minus);


    ~Axis();

    bool move(float mm, float mm_min);

    float get_position() { return this->position; };
    bool get_endstop_plus() { return !gpio_get(this->endstop_pin_plus); };
    bool get_endstop_minus() { return !gpio_get(this->endstop_pin_minus); };
    uint8_t get_endstop_pin_plus() { return this->endstop_pin_plus;};
    uint8_t get_endstop_pin_minus() { return this->endstop_pin_minus;};

    void dereference(){this->referenced = false;};
    void reference(float position = 0);
    bool is_referenced() { return this->referenced;};

    void stop(){this->stepper.stop();};

    bool is_moving();


};


