#include "CNC_Controler.h"


// nicht verwendet
float CNC_Controler::direction_similarity(float x_mm, float y_mm, float x2_mm, float y2_mm){

    float dist = sqrt((x_mm * x_mm) + (y_mm * y_mm));
    float dist2 = sqrt((x2_mm * x2_mm) + (y2_mm * y2_mm));
    
    // Wenn die Distanz 0 ist, gibt es nichts zu bewegen
    if(dist == 0 || dist2 == 0) return 1;
    
    // Berechne die Anteile der Bewegung in x- und y-Richtung
    float tmp_x_pitch = abs(x_mm) / dist;
    float tmp_y_pitch = abs(y_mm) / dist;

    float tmp_x2_pitch = abs(x2_mm) / dist2;
    float tmp_y2_pitch = abs(y2_mm) / dist2;

    // Berechne das Skalarprodukt der beiden Richtungsvektoren
    float dot_product = tmp_x_pitch * tmp_x2_pitch + tmp_y_pitch * tmp_y2_pitch;

    // Normalisiere das Ergebnis auf den Bereich [0, 1]
    return (dot_product + 1.0) / 2.0;

}

// Führt eine automatische Referenzfahrt für alle drei Achsen durch (X, Y, Z)
// Ablauf: mechanischer Anschlag → Abtasten → Rückfahren → Referenz setzen
void CNC_Controler::reference(){

    // Temporäres Zwischenspeichern der Modi
    bool temp = this->abs_mode;
    bool temp2 = this->endstop_check_active;

    // Endstop-Überwachung deaktivieren und Absolutmodus ausschalten
    this->endstop_check(false);
    this->abs_mode = false;

    // Warten bis aktuelle Bewegungen beendet sind
    while(this->stop_time_xy > time_us_32())
        tight_loop_contents();
    while(this->stop_time_z > time_us_32())
        tight_loop_contents();

    // X-Achse grob in positive Richtung verfahren
    this->move_xy(300,0,300);
    while(this->x_axis.get_endstop_plus()) // Warten auf Endschalter
        tight_loop_contents();
    this->hard_stop();

    // Zurückziehen und langsames Anfahren zur genauen Referenzposition
    this->move_xy(-4, 0, 300);
    sleep_ms(500);
    this->move_xy(5, 0, 50);
    while(this->x_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    // Setze Referenzpunkt der X-Achse (Wert ggf. anpassen)
    this->x_axis.reference(173);
    this->move_xy(-4, 0, 200);

    // Y-Achse Referenzfahrt identisch zu X
    this->move_xy(0, 300, 300);
    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_xy(0, -4, 200);
    sleep_ms(500);
    this->move_xy(0, 5, 50);
    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->y_axis.reference(180);
    this->move_xy(0, -4, 300);

    // Z-Achse Referenzfahrt
    this->move_z(40, 200);
    while(this->z_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_z(-5, 300);
    sleep_ms(500);
    this->move_z(11, 50);
    while(this->z_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->z_axis.reference(5); // Z-Referenzwert setzen
    this->move_z(-5, 300);

    // Kurze Pause und Wiederherstellung der ursprünglichen Modi
    sleep_ms(3000);
    this->abs_mode = temp;
    this->endstop_check(temp2);
    this->hard_stop();
}

// Setzt den Absolut-/Relativmodus für Bewegungen
void CNC_Controler::set_abs_mode(bool abs){
    this->abs_mode = abs;
}