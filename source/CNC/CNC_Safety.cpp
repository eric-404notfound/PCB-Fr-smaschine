#include "CNC_Controler.h" // Einbinden der Bibliothek

extern CNC_Controler cnc;

void CNC_Controler::endstop_callback(uint gpio, uint32_t events){

    if (gpio == cnc.x_axis.get_endstop_pin_minus() || cnc.x_axis.get_endstop_pin_minus()) // Abfrage der Endschalter für die X Achse
        cnc.x_axis.stop();                                                    // Wenn einer der Endschalter betätigt wurde Stoppt die X Achse
    if (gpio==cnc.y_axis.get_endstop_pin_minus() || cnc.y_axis.get_endstop_pin_minus())
        cnc.y_axis.stop();
    if (gpio==cnc.z_axis.get_endstop_pin_minus() || cnc.z_axis.get_endstop_pin_minus())
        cnc.z_axis.stop();

    cnc.hard_stop(); // CNC hält sofort alles an, auch die Spindel
    cnc.endstop_hit = true; // Alle Achsen verlieren Referenz und Stoppen
    cnc.x_axis.dereference(); // Löschen der Referenz der X-Achse
    cnc.y_axis.dereference(); // Löschen der Referenz der Y-Achse
    cnc.z_axis.dereference(); // Löschen der Referenz der Z-Achse
    
}

void CNC_Controler::endstop_check(bool on_off){

    this->endstop_check_active = on_off; // Speichern des Einschalten der Überwachung der Endschalter

    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); // Interupts die dauerhaft die Endschalter Überprüfen
    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); 
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);

    this->endstop_hit = false; // Wenn kein Endschalter betätigt wird läuft das Programm normal weiter

}


