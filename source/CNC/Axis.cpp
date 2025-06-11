#include "Axis.h"


// Konstruktor der Klasse Axis
// Initialisiert eine Achse mit den angegebenen Parametern:
// - length: mechanische Länge der Achse (in mm oder Einheiten)
// - step_pin: GPIO-Pin für Step-Signal des Schrittmotortreibers
// - dir_pin: GPIO-Pin für Richtungssignal des Schrittmotortreibers
// - axis: Bezeichner der Achse (z. B. 'X', 'Y', 'Z')
// - endstop_pin_plus: GPIO-Pin für den Endschalter am positiven Ende
// - endstop_pin_minus: GPIO-Pin für den Endschalter am negativen Ende
Axis::Axis(float length, uint8_t step_pin, uint8_t dir_pin, char axis, uint8_t endstop_pin_plus, uint8_t endstop_pin_minus)
    :stepper(step_pin, dir_pin, axis),                // Initialisierung des Stepper-Objekts mit Pins und Achsenbezeichnung
     axisLength(length),                             // Speichern der Achsenlänge
     axis_name(axis),                                // Speichern der Achsenbezeichnung
     step_pin(step_pin), dir_pin(dir_pin),           // Speicherung der Step/Dir-Pins
     endstop_pin_plus(endstop_pin_plus),             // Speicherung des Plus-Endschalters
     endstop_pin_minus(endstop_pin_minus)            // Speicherung des Minus-Endschalters
{
    // Initialisierung und Konfiguration des Plus-Endschalters als Eingang mit Pull-Up
    gpio_init(endstop_pin_plus);
    gpio_set_dir(endstop_pin_plus , GPIO_IN);
    gpio_pull_up(endstop_pin_plus);

    // Initialisierung und Konfiguration des Minus-Endschalters als Eingang mit Pull-Up
    gpio_init(endstop_pin_minus);
    gpio_set_dir(endstop_pin_minus , GPIO_IN);
    gpio_pull_up(endstop_pin_minus);
    
    this->position = 0; // Initiale Position der Achse auf 0 setzen
}

// Destruktor der Klasse Axis
Axis::~Axis(){}


// Führt eine Bewegung der Achse um die angegebene Distanz (mm) mit einer definierten Geschwindigkeit (mm/min) aus
// Rückgabe: true bei erfolgreicher Ausführung, false bei Grenzwertverletzung oder Fehler
bool Axis::move(float mm, float mm_min){

    // Sicherheitsabfrage: Bewegungsgrenze überschritten
    if (mm < -MAX_DISTANCE || mm > MAX_DISTANCE){
        printf("Error MaxD: Axis:%c, mm:%f <-> %f max\n", this->axis_name, mm, MAX_DISTANCE);
        return false;
    }

    // Sicherheitsabfrage: Geschwindigkeit außerhalb gültiger Grenzen
    if (mm_min < 0 || mm_min > MAX_AXIS_SPEED){
        printf("Error: Axis:%c, mm_min:%f <-> %f max\n", this->axis_name, mm_min, MAX_AXIS_SPEED);
        return false;
    }

    // Bestimmung der Bewegungsrichtung (0 = positiv, 1 = negativ)
    bool direction = mm < 0 ? 1 : 0;

    // Positionsprüfung nur bei referenzierter Achse
    if(this->referenced){
        this->position += mm;
        // Prüfung ob Position außerhalb der Achsenlänge liegt
        if(this->position > this->axisLength || this->position < 0){
            printf("Error: Axis:%c, position:%f <-> %f max\n", this->axis_name, this->position, this->axisLength);
            this->position -= mm;
            return false;
        }
    }

    // Betrag für weitere Berechnung (Vorzeichen bereits in Richtung erfasst)
    if(direction)
        mm = -mm;

    // Umrechnung in Schritte (float → int32_t)
    uint32_t steps = static_cast<uint32_t>(mm * STEPS_PER_MM * MICROSTEPS);

    // Sicherheitsabfrage: Schrittanzahl überschreitet zulässigen Wert (15 Bit)
    if (steps > 0x7FFF){
        printf("Error: Axis:%c, steps:%d <-> %d max\n", this->axis_name, steps, 0x7FFF);
        return false;
    }

    // Berechnung der Verzögerung pro Schritt basierend auf der gewünschten Vorschubgeschwindigkeit
    // Formel: (Takte/s * 60) / (Schritte/mm * mm/min)
    uint32_t delay = 0;
    if(!mm_min == 0) // Hinweis: Logikfehler (sollte mm_min != 0 sein)
        delay = static_cast<uint32_t>((double)(100000 * 60) / (double)(STEPS_PER_MM * static_cast<double>(mm_min)));

    // Sicherheitsabfrage: Delay überschreitet 16 Bit
    if (delay > 0xFFFF){
        printf("Error: Axis:%c, delay:%d <-> %d max\n / mm_min:%f \n ", this->axis_name, delay, 0xFFFF, mm_min);
        return false;
    }

    // Kommandowert codieren und an Stepper übergeben
    // Format: [Richtung(1 Bit)][Schritte(15 Bit)][Delay(16 Bit)]
    this->stepper.move(((direction & 0x1) << 31) | ((steps & 0x7FFF) << 16) | (delay & 0xFFFF));
    this->total_moved += mm; // Gesamtbewegung aktualisieren

    return true;
}

// Setzt die aktuelle Position der Achse und markiert sie als referenziert
void Axis::reference(float position){
    this->referenced = true;
    this->position = position;
}

// Gibt true zurück, solange eine Bewegung aktiv ist (Status vom Stepper-Modul)
bool Axis::is_moving(){
    return this->stepper.processing();
}
