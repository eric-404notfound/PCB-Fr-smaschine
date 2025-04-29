#include "CNC_Controler.h"

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

void CNC_Controler::reference(){

    bool temp = this->abs_mode, temp2 = this->endstop_check_active;
    
    this->endstop_check(false);
    this->abs_mode = false;

    while(this->stop_time > time_us_64())
        tight_loop_contents();

    this->x_axis.stop();
    this->y_axis.stop();
    
    this->move_xy(300,0,500);
    
    while(this->x_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_xy(-4, 0,500);
    sleep_ms(500);
    this->move_xy(5,0,50);

    while(this->x_axis.get_endstop_plus())
        tight_loop_contents();

    this->x_axis.stop();
    this->y_axis.stop();

    this->x_axis.reference(190); //wert muss noch angepasst werden
    this->move_xy(-4, 0,500);


    this->move_xy(0,300,500);

    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_xy(0, -4,500);
    sleep_ms(500);
    this->move_xy(0, 5,50);

    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();

    this->y_axis.stop();
    this->x_axis.stop();

    this->y_axis.reference(290); //wert muss noch angepasst werden
    this->move_xy(0,-4,500);

    this->move_z(-50, 500);
    while(this->z_axis.get_endstop_minus())
        tight_loop_contents();
    this->hard_stop();

    this->move_z(10, 500);
    sleep_ms(500);
    this->move_z(-11, 50);
    while(this->z_axis.get_endstop_minus())
        tight_loop_contents();
    this->hard_stop();
    this->move_z(20, 500);

    this->z_axis.reference(19); //wert muss noch angepasst werden

    this->abs_mode = temp;
    this->endstop_check(temp2);

}