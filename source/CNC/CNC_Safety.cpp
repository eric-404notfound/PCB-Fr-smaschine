#include "CNC_Controler.h" // Einbinden der Bibliothek

extern CNC_Controler cnc;

void CNC_Controler::endstop_callback(uint gpio, uint32_t events){

    cnc.hard_stop(); // CNC hält sofort alles an, auch die Spindel
    cnc.endstop_hit = true; // Alle Achsen verlieren Referenz und Stoppen
    cnc.x_axis.dereference(); // Löschen der Referenz der X-Achse
    cnc.y_axis.dereference(); // Löschen der Referenz der Y-Achse
    cnc.z_axis.dereference(); // Löschen der Referenz der Z-Achse
    
}

void CNC_Controler::endstop_check(bool on_off){

    this->endstop_check_active = on_off; // Einschalten der Überwachung der Endschalter

    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); // Interupts die dauerhaft die Endschalter Überprüfen
    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); 
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);

    this->endstop_hit = false; // Wenn kein Endschalter betätigt wird läuft das Programm normal weiter

}

