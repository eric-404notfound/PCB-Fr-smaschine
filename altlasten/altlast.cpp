void CNC_Controler::clever_move_xy(move_command* commands, uint8_t n){

    float c_mm_min = 0;
    float C_x_mm = 0;
    float C_y_mm = 0;

    for(uint8_t i = 0; i < n; i++){

        

        // Wenn es eine Verzögerungsstrecke gibt, berechne die Ähnlichkeit der Richtungen und dekrementiere die Geschwindigkeit
        if(this->dcc_distance > 0){
            float similarity = this->direction_similarity(C_x_mm, C_y_mm, commands[i].x_mm, commands[i].y_mm);
            this->decc_xy(this->dcc_distance, similarity);
        }

        c_mm_min = commands[i].mm_min;
        C_x_mm = commands[i].x_mm;
        C_y_mm = commands[i].y_mm;

        float distance = sqrt(C_x_mm * C_x_mm + C_y_mm * C_y_mm);

        // Setze die Richtung der Bewegung
        this->set_xy_direction(C_x_mm, C_y_mm);
        // Wenn die Distanz kleiner als die doppelte Beschleunigungs-/Verzögerungsstrecke ist, beschleunige und verzögere entsprechend
        if(distance < ACC_DCC_DISTANCE * 2){
            this->acc_xy(distance / 2, c_mm_min);
            this->dcc_distance = distance / 2;
            return;
        }
        else{

            // Beschleunige auf die Zielgeschwindigkeit
            this->acc_xy(ACC_DCC_DISTANCE, c_mm_min);
            distance -= ACC_DCC_DISTANCE * 2;
            // Bewege die Achsen um die verbleibende Strecke
            this->raw_xy(distance);
            this->dcc_distance = distance;
        }

    }
  
}

void CNC_Controler::acc_xy(float mm, float mm_min){

    float initial_speed = this->is_mm_min;

    // Wenn die Anfangsgeschwindigkeit größer als die Zielgeschwindigkeit ist, bewege die Achsen ohne Beschleunigung
    if(initial_speed > mm_min){
        this->raw_xy(mm);
        return;
    };

    float acceleration = mm_min - initial_speed;

    // Erhöhe die Geschwindigkeit in 10 Schritten
    for(uint8_t i = 1; i < 11; i++){
        this->set_xy_speed(initial_speed + pow(100,  0.1 * i - 1) * acceleration);
        this->raw_xy(0.1 * mm);
    }

}

void CNC_Controler::decc_xy(float mm, float mm_min){

    float initial_speed = this->is_mm_min;

    // Wenn die Anfangsgeschwindigkeit kleiner als die Zielgeschwindigkeit ist, bewege die Achsen ohne Verzögerung
    if(initial_speed < mm_min){
        this->raw_xy(mm);
    };

    float deceleration = initial_speed - mm_min;

    // Verringere die Geschwindigkeit in 10 Schritten
    for(uint8_t i = 1; i < 10; i++){
        this->set_xy_speed(initial_speed - pow(100,  0.1 * i - 1) * deceleration);
        this->raw_xy(0.1 * mm);
    }
}