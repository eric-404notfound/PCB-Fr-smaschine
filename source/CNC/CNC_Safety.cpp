#include "CNC_Controler.h"

extern CNC_Controler cnc;

void CNC_Controler::endstop_callback(uint gpio, uint32_t events){

    cnc.hard_stop();
    cnc.endstop_hit = true; // Alle Achsen verlieren Reference und Stoppen
    cnc.x_axis.dereference();
    cnc.y_axis.dereference();
    cnc.z_axis.dereference();
    
}

void CNC_Controler::endstop_check(bool on_off){

    this->endstop_check_active = on_off;

    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); // Interupts die Regelmäßig die Endschalter ÜBerprüfen
    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); // 
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);

    this->endstop_hit = false;

}

