#include "CNC_Controler.h"



CNC_Controler::CNC_Controler(Axis &x, Axis &y, Axis &z): x_axis(x), y_axis(y), z_axis(z), stop_time(time_us_64()) {
    // Konstruktor
};

CNC_Controler::~CNC_Controler() {
    // Destruktor
    printf("CNC_Controler destructor\n");
    while(this->stop_time > time_us_64())
        tight_loop_contents();

    while(this->x_axis.is_moving() || this->y_axis.is_moving())
        tight_loop_contents();

}

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

    #if DEBUG > 2
        printf("x_pitch: %f y_pitch: %f\n", this->x_pitch, this->y_pitch);
    #endif
 
}

void CNC_Controler::set_xy_speed(float mm_min){

    // Skaliere die Geschwindigkeit entsprechend der Bewegung
    this->x_mm_min = mm_min * abs(this->x_pitch);
    this->y_mm_min = mm_min * abs(this->y_pitch);
    this->is_mm_min = mm_min;
}

void CNC_Controler::raw_xy(float mm){

    float calls = mm / static_cast<float>(MAX_DISTANCE);

    if (this->stop_time < time_us_64())
        this->stop_time = time_us_64(); //+ 500000;
    
    if(x_pitch != 0)
        this->stop_time += static_cast<uint64_t>((mm * abs(this->x_pitch)) *60*1000*1000 / this->x_mm_min);
    else
        this->stop_time += static_cast<uint64_t>((mm * abs(this->y_pitch)) *60*1000*1000 / this->y_mm_min); 

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

}

void CNC_Controler::move_xy(float x_mm, float y_mm, float mm_min){


    // Wenn der absolute Modus aktiviert ist, subtrahiere die aktuelle Position
    if (this->abs_mode){
        x_mm -= this->x_axis.get_position();
        y_mm -= this->y_axis.get_position();
    }

    // Setze die Richtung der Bewegung
    this->set_xy_direction(x_mm, y_mm);
    this->set_xy_speed(mm_min);

    float distance = sqrt(x_mm * x_mm + y_mm * y_mm);
    // Wenn die Distanz 0 ist, gibt es nichts zu bewegen
    if(distance == 0)
        return;

    // Bewege die Achsen um die verbleibende Strecke
    this->raw_xy(distance);
}

void CNC_Controler::move_z(float mm, float mm_min){

    if (this->abs_mode)
        mm -= this->z_axis.get_position();
    
    // Wenn sich x oder y bewegen, warte bis sie fertig sind
    while(this->x_axis.is_moving() || this->y_axis.is_moving())
        tight_loop_contents();
    
    // Wenn Theoretisch keine Bewegung stattfindet mach weiter
    #if DEBUG >2
        printf("Waiting %dms" , stop_time - time_us_64());
    #endif

    while(this->stop_time > time_us_64())
        tight_loop_contents();

    float steps = mm / static_cast<float>(MAX_DISTANCE);

    // Bewege die Achsen in Schritten von MAX_DISTANCE
    for(;steps > 1; steps--){
        if(!this->z_axis.move(MAX_DISTANCE, mm_min))
            printf("Error: z // mm: %f speed:%f \n", MAX_DISTANCE, mm_min);
        
    }

    // Bewege die Achsen um den Rest der Strecke
    if(!this->z_axis.move(MAX_DISTANCE * steps, mm_min))
        printf("Error: z\n");

} 

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

    
    int j = 0;
    for (int i = 0; i < Program_gcode_len && !this->endstop_hit; i++)
        if (Program_gcode[i] != 0x0a)
            continue;
        else{

            std::string G_Funktion_str(Program_gcode[j], i - j);
            j = i + 2;

            GCODE Befehl = this->parse_GCODE(G_Funktion_str);

            float speed = 0;

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

void CNC_Controler::hard_stop(){
    this->stop_time = time_us_64();
    this->x_axis.stop();
    this->y_axis.stop();
    this->z_axis.stop();

}

