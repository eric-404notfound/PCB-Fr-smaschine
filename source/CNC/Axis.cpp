#include "Axis.h"


Axis::Axis(float length, uint8_t step_pin, uint8_t dir_pin, char axis, uint8_t endstop_pin_plus, uint8_t endstop_pin_minus)
    :stepper(step_pin, dir_pin, axis), axisLength(length), axis_name(axis), step_pin(step_pin), dir_pin(dir_pin), endstop_pin_plus(endstop_pin_plus), endstop_pin_minus(endstop_pin_minus){
    
    gpio_init(endstop_pin_plus);
    gpio_set_dir(endstop_pin_plus , GPIO_IN);
    gpio_pull_up(endstop_pin_plus);

    gpio_init(endstop_pin_minus);
    gpio_set_dir(endstop_pin_minus , GPIO_IN);
    gpio_pull_up(endstop_pin_minus);
    
    this->position = 0;
}

Axis::~Axis(){}

bool Axis::move(float mm, float mm_min){

    //temporary 40.96 bei 8 microsteps -> 81.92 bei 4 microsteps -> 163.84 bei 2 microsteps -> 327.68 bei 1 microstep
    if (mm < -MAX_DISTANCE || mm > MAX_DISTANCE){
        printf("Error: Axis:%c, mm:%f <-> %f max\n", this->axis_name, mm, MAX_DISTANCE);
        return false;

    }
        
    
    if (mm_min < 0 || mm_min > MAX_AXIS_SPEED){

        printf("Error: Axis:%c, mm_min:%f <-> %f max\n", this->axis_name, mm_min, MAX_AXIS_SPEED);
        return false;
    }
        
    //printf("Axe:%c, start:%f <-> %f:end \n", this->axis_name, this->position, this->position += mm);


    bool direction = mm < 0 ? 1 : 0;
    if(this->referenced){
        this->position += mm;
        if(this->position > this->axisLength || this->position < 0){

            printf("Error: Axis:%c, position:%f <-> %f max\n", this->axis_name, this->position, this->axisLength);
            this->position -= mm;

            return false;
        }
    }
    
    if(direction)
        mm = -mm;

    uint32_t steps = static_cast<uint32_t>(mm * STEPS_PER_MM * MICROSTEPS);

    if (steps > 0x7FFF){
        printf("Error: Axis:%c, steps:%d <-> %d max\n", this->axis_name, steps, 0x7FFF);
        return false;
    }

        
    
    //  2MHz = Tacktrate des PIO -> 100kHz = 20us pro Periode
    //  8 Microsteps -> 160takte pro Schritt
    uint32_t delay = 0;
    if(!mm_min == 0)
        delay = static_cast<uint32_t>((double)(100000 * 60) / (double)((double)STEPS_PER_MM * static_cast<double>(mm_min)));

    if (delay > 0xFFFF){

        printf("Error: Axis:%c, delay:%d <-> %d max\n / mm_min:%f \n ", this->axis_name, delay, 0xFFFF, mm_min);
        return false;
    }

    //printf("Axis:%c, mm: %f, mm_min: %f direction:%d \n", this->axis_name,  mm, mm_min, direction);
    this->stepper.move(((direction & 0x1) << 31) | ((steps & 0x7FFF) << 16) | (delay & 0xFFFF));
    this->total_moved += mm;

    return true;
};

void Axis::reference(float position){

    this->referenced = true;
    this->position = position;

}

bool Axis::is_moving(){
    return this->stepper.processing();
};
