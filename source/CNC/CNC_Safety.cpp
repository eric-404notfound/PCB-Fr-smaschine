#include "CNC_Controler.h" // Einbinden der Bibliothek

extern CNC_Controler cnc; // Globale CNC-Controller-Instanz


// Callback-Funktion für Endschalter (wird durch GPIO-Interrupts aufgerufen)
void CNC_Controler::endstop_callback(uint gpio, uint32_t events){

    // Prüfung ob X-Endschalter (Minus) betätigt wurde
    if (gpio == cnc.x_axis.get_endstop_pin_minus() || cnc.x_axis.get_endstop_pin_minus())
        cnc.x_axis.stop(); // Achse sofort stoppen

    // Prüfung ob Y-Endschalter (Minus) betätigt wurde
    if (gpio == cnc.y_axis.get_endstop_pin_minus() || cnc.y_axis.get_endstop_pin_minus())
        cnc.y_axis.stop();

    // Prüfung ob Z-Endschalter (Minus) betätigt wurde
    if (gpio == cnc.z_axis.get_endstop_pin_minus() || cnc.z_axis.get_endstop_pin_minus())
        cnc.z_axis.stop();

    cnc.hard_stop(); // Not-Halt aller Achsen und ggf. Spindel
    cnc.endstop_hit = true; // Status setzen: Endschalter wurde ausgelöst

    // Referenzierung aller Achsen zurücksetzen
    cnc.x_axis.dereference();
    cnc.y_axis.dereference();
    cnc.z_axis.dereference();
}

// Aktivieren oder Deaktivieren der Endschalterüberwachung
void CNC_Controler::endstop_check(bool on_off){

    this->endstop_check_active = on_off; // interner Status speichern

    // GPIO-Interrupts für alle Endschalter konfigurieren (fallende Flanke)
    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->x_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback); 
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->y_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_minus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);
    gpio_set_irq_enabled_with_callback(this->z_axis.get_endstop_pin_plus(), GPIO_IRQ_EDGE_FALL, on_off, &this->endstop_callback);

    this->endstop_hit = false; // Zustand zurücksetzen: kein Endschalter aktiv
}


