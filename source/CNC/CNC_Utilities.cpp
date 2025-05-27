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

    bool temp = this->abs_mode;
    bool temp2 = this->endstop_check_active;
    
    this->endstop_check(false);
    this->abs_mode = false;

    while(this->stop_time_xy > time_us_32())
        tight_loop_contents();

    while(this->stop_time_z > time_us_32())
        tight_loop_contents();

    this->hard_stop();
    
    this->move_xy(300,0,300);
    
    while(this->x_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_xy(-4, 0,300);
    sleep_ms(500);
    this->move_xy(5,0,50);

    while(this->x_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->x_axis.reference(185); //wert muss noch angepasst werden
    this->move_xy(-4, 0,200);


    this->move_xy(0,300,300);

    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();
    this->hard_stop();

    this->move_xy(0, -4,200);
    sleep_ms(500);
    this->move_xy(0, 5,50);

    while(this->y_axis.get_endstop_plus())
        tight_loop_contents();

    this->hard_stop();

    this->y_axis.reference(215); //wert muss noch angepasst werden
    this->move_xy(0,-4,300);

    this->move_z(-50, 200);
    while(this->z_axis.get_endstop_minus())
        tight_loop_contents();
    this->z_axis.stop();

    this->move_z(5, 300);
    sleep_ms(500);
    this->move_z(-11, 50);
    while(this->z_axis.get_endstop_minus())
        tight_loop_contents();
    this->hard_stop();
    this->z_axis.reference(35);
    this->move_z(8, 300);
    

     //wert muss noch angepasst werden
    sleep_ms(3000);
    this->abs_mode = temp;
    this->endstop_check(temp2);
    this->hard_stop();
}

void CNC_Controler::set_abs_mode(bool abs){
    this->abs_mode = abs;
}