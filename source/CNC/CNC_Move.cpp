#include "CNC_Controler.h"



CNC_Controler::CNC_Controler(Axis &x, Axis &y, Axis &z): x_axis(x), y_axis(y), z_axis(z), stop_time_xy(time_us_64()) {
    // Konstruktor
};

// Destruktor: Warten bis alle Bewegungen abgeschlossen sind, dann Endstop-Zustand prüfen
CNC_Controler::~CNC_Controler() {
    printf("CNC_Controler destructor\n");
    while(this->stop_time_xy > time_us_32())
        tight_loop_contents();

    while(this->x_axis.is_moving() || this->y_axis.is_moving())
        tight_loop_contents();
    
    endstop_check(true);

}

// Richtung der XY-Bewegung als normierte Vektoren berechnen
void CNC_Controler::set_xy_direction(float x_mm, float y_mm){

    float dist = sqrt(x_mm * x_mm + y_mm * y_mm);
    
    // Wenn die Distanz 0 ist, gibt es nichts zu bewegen
    if(dist == 0){
        this->x_pitch = 0;
        this->y_pitch = 0;
        return;
    };
 
    // Berechne die Anteile der Bewegung in x- und y-Richtung
    this->x_pitch = x_mm / dist;
    this->y_pitch = y_mm / dist;
    
 
}

// Geschwindigkeit auf x- und y-Komponente verteilen
void CNC_Controler::set_xy_speed(float mm_min){

    // Skaliere die Geschwindigkeit entsprechend der Bewegung
    this->x_mm_min = mm_min * abs(this->x_pitch);
    this->y_mm_min = mm_min * abs(this->y_pitch);
    this->is_mm_min = mm_min;
}


// Führt eine Rohbewegung in XY um eine bestimmte Strecke aus (ggf. in Teilstücke)
void CNC_Controler::raw_xy(float mm){

    float calls = mm / static_cast<float>(MAX_DISTANCE);

    if (this->stop_time_xy < time_us_32())
        this->stop_time_xy = time_us_32(); //+ 500000;
    
    if(x_pitch != 0)
        this->stop_time_xy += static_cast<uint32_t>((abs(mm) * abs(this->x_pitch)) *60*1000*1000 / this->x_mm_min);
    else
        this->stop_time_xy += static_cast<uint32_t>((abs(mm) * abs(this->y_pitch)) *60*1000*1000 / this->y_mm_min); 

    x_pos += mm * x_pitch;
    y_pos += mm * y_pitch;
    // Bewege die Achsen in Schritten von MAX_DISTANCE
    for(;calls > 1; calls--){
        if(!this->x_axis.move(MAX_DISTANCE * x_pitch, this->x_mm_min))
            printf("Error: x // mm: %f speed:%f \n", MAX_DISTANCE * x_pitch, this->x_mm_min);
        if(!this->y_axis.move(MAX_DISTANCE * y_pitch, this->y_mm_min))
            printf("Error: y // mm: %f speed:%f \n", MAX_DISTANCE * y_pitch, this->y_mm_min);
    }

    // Bewege die Achsen um den Rest der Strecke
    if(!this->x_axis.move(MAX_DISTANCE * calls * this->x_pitch, this->x_mm_min))
        printf("Error: x\n");

    if(!this->y_axis.move(MAX_DISTANCE * calls * this->y_pitch, this->y_mm_min))
        printf("Error: y\n");
    printf("X Achse fährt %fmm mit %fmm/min\n",mm * abs(this->x_pitch), this->x_mm_min);
    printf("Y Achse fährt %fmm mit %fmm/min\n",mm * abs(this->y_pitch), this->y_mm_min);
}


// Kombinierte XY-Bewegung mit Geschwindigkeitsvorgabe
void CNC_Controler::move_xy(float x_mm, float y_mm, float mm_min){
    if (this->endstop_hit)
        return;

    // Wenn der absolute Modus aktiviert ist, subtrahiere die aktuelle Position
    if (this->abs_mode){
        x_mm -= this->x_pos;
        y_mm -= this->y_pos;
    }

    // Setze die Richtung der Bewegung
    this->set_xy_direction(x_mm, y_mm);
    this->set_xy_speed(mm_min);

    float distance = sqrt(x_mm * x_mm + y_mm * y_mm);
    // Wenn die Distanz 0 ist, gibt es nichts zu bewegen
    if(distance == 0)
        return;

    //while (this->z_axis.is_moving())
        //tight_loop_contents();
    

    while(this->stop_time_z > time_us_32())
        tight_loop_contents();
    // Bewege die Achsen um die verbleibende Strecke
    this->raw_xy(distance);
}

// Bewegung der Z-Achse mit Zeitplanung und Zerlegung in Teilbewegungen
void CNC_Controler::move_z(float mm, float mm_min){

    if (this->endstop_hit)
        return;
    // axe musste gedreht werden 
    mm = -mm;
    
    if (this->abs_mode){
        mm -= this->z_pos;
    }
    
    this->z_pos += mm;
    // Wenn sich x oder y bewegen, warte bis sie fertig sind
    //while(this->x_axis.is_moving() || this->y_axis.is_moving())
        //tight_loop_contents();
    
    // Wenn Theoretisch keine Bewegung stattfindet mach weiter
    #if DEBUG >2
        printf("Waiting %dms" , stop_time_xy - time_us_32());
    #endif

    while(this->stop_time_xy > time_us_32())
        tight_loop_contents();

    if(this->stop_time_z <  time_us_32())
        this->stop_time_z = time_us_32();

    this->stop_time_z += static_cast<uint32_t>((abs(mm) * 1) *60*1000*1000 / mm_min);
    float steps = mm / static_cast<float>(MAX_DISTANCE);

    printf("Z Achse fährt %fmm mit %fmm/min\n",mm,mm_min );

    // Bewege die Achsen in Schritten von MAX_DISTANCE
    for(;steps > 1; steps--){
        if(!this->z_axis.move(MAX_DISTANCE, mm_min))
            printf("Error: z // mm: %f speed:%f \n", MAX_DISTANCE, mm_min);
        
    }

    // Bewege die Achsen um den Rest der Strecke
    if(!this->z_axis.move(MAX_DISTANCE * steps, mm_min))
        printf("Error: z\n");

} 
// Setzt die absolute Position intern (ohne Bewegung)
void CNC_Controler::set_pos(float x, float y, float z){
    
    x_pos = x;
    y_pos = y;
    z_pos = z;
}

// GCode-Programm aus dem Dateisystem auslesen und ausführen
void CNC_Controler::run_programm(){

    FATFS fs;
    FRESULT fr;
    DIR dir;
    FILINFO fno;
    FIL file;
    UINT br;
    std::string filename;
    
    fr = f_opendir(&dir, "0:");
    if (fr != FR_OK) {
        printf("f_opendir error (%d)\n", fr);
        return;
    }

    while(true){
        fr = f_readdir(&dir, &fno);

        if (fr != FR_OK){
            printf("f_readdir error (%d)\n", fr);
            return;
        }
        if(fno.fname[0] == 0){
            break;
        }

        if (fno.fattrib & AM_DIR){
            printf("Directory: %s\n", fno.fname);
        } 
        else {
            auto key = std::string(".gcode");
            filename = std::string(fno.fname); //.gcode auslesen
            std::size_t found = filename.rfind(key);
            if (found!=std::string::npos)
                break;

            printf("File:%s\n", fno.fname);
        }
    }
    f_closedir(&dir);
    f_open(&file, filename.c_str(),FA_READ);

    char line[100]; 
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    float speed = 0;
    while (f_gets(line, sizeof(line), &file)) {

        std::string G_Funktion_str(line);
        printf(G_Funktion_str.c_str());
        
        GCODE Befehl = this->parse_GCODE(G_Funktion_str);
        printf("Daraus folgt %c %d " ,Befehl.Befehl.Type, Befehl.Befehl.Funktion);
        if(Befehl.Parameter.X.has_value())
            printf("X%f ",Befehl.Parameter.X.value());
        if(Befehl.Parameter.Y.has_value())
            printf("Y%f ",Befehl.Parameter.Y.value());
        if(Befehl.Parameter.Z.has_value())
            printf("Z%f ",Befehl.Parameter.Z.value());
        if (Befehl.Parameter.F.has_value())
            printf("F%f ", Befehl.Parameter.F.value());
        printf("\n");
        


        switch (Befehl.Befehl.Type)
        {
        case 'G':
            switch (Befehl.Befehl.Funktion){
            
            case 0:
                // G00: Schnellbewegung
                if (Befehl.Parameter.X.has_value() && Befehl.Parameter.Y.has_value()){
                    this->move_xy(Befehl.Parameter.X.value(), Befehl.Parameter.Y.value(), G00_MM_MIN);
                }
                else if (Befehl.Parameter.Z.has_value()){
                    this->move_z(Befehl.Parameter.Z.value(), G00_MM_MIN);
                }

                break;
            
            case 1:

                if (Befehl.Parameter.F.has_value())
                    speed = Befehl.Parameter.F.value();
                // G01: Lineare Interpolation
                if (Befehl.Parameter.X.has_value() && Befehl.Parameter.Y.has_value()){
                    this->move_xy(Befehl.Parameter.X.value(), Befehl.Parameter.Y.value(),speed);
                }
                else if (Befehl.Parameter.Z.has_value()){
                    this->move_z(Befehl.Parameter.Z.value(), speed);
                }
                break;
            
            case 30:
                // G30: Programmende
                //Referenzfahrt
                break;
            
            case 90:
                // G90: Absolute Positionierung
                this->abs_mode = true;
                break;
            
            case 91:
                // G91: Relative Positionierung
                this->abs_mode = false;
                break;
            
            default:
                break;
            }

            break;
        
        case 'M':
            /* code */
            switch (Befehl.Befehl.Funktion)
            {
            case 3:
                #if SPINDEL_ADJUSTABEL
                if (Befehl.Parameter.S.has_value())
                    this->activate_spindle(Befehl.Parameter.S.value());
                else
                    this->activate_spindle(0);
                #else
                    this->activate_spindle(true);
                #endif

                break;

            
            case 4:
                #if SPINDEL_ADJUSTABEL
                if (Befehl.Parameter.S.has_value())
                    this->activate_spindle(-Befehl.Parameter.S.value());
                else
                    this->activate_spindle(0);
                
                #else
                    this->activate_spindle(true);
                #endif

                break;

            case 5:
                #if SPINDEL_ADJUSTABEL
                    this->activate_spindle(0);
                #else
                    this->activate_spindle(false);
                #endif

                break;
            
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

// Sofortiger Stopp aller Achsbewegungen
void CNC_Controler::hard_stop(){
    this->stop_time_xy = time_us_32();
    this->stop_time_z = time_us_32();
    this->x_axis.stop();
    this->y_axis.stop();
    this->z_axis.stop();

}

// Aktiviert oder deaktiviert die Spindel (nur Debug-Ausgabe)
void CNC_Controler::activate_spindle(bool onoff){
    printf("%d",onoff);
};
