
#pragma once

#define DEBUG 3

#include "Axis.h"
#include "sd_card.h"
#include "ff.h"


#include "math.h"
#include <string>
#include <optional>
#include "hardware/gpio.h"

#include "Program.h"


#define ACC_DCC_DISTANCE 1.0
#define G00_MM_MIN 800.0

#define SPINDEL_ADJUSTABEL false


struct GCODE_BEFEHL{
    char Type;
    uint16_t Funktion;
};

struct GCODE_PARAMETER{
    std::optional<float> X;
    std::optional<float> Y;
    std::optional<float> Z;
    std::optional<float> F;
    std::optional<float> S;
};

struct GCODE{
    GCODE_BEFEHL Befehl;
    GCODE_PARAMETER Parameter;
};

class CNC_Controler{

private:

    bool abs_mode = false;

    float spindle_speed = 0;

    Axis &x_axis;
    float x_mm_min = 0;
    float x_pitch = 0;

    Axis &y_axis;
    float y_mm_min = 0;
    float y_pitch = 0;

    Axis &z_axis;
    float is_mm_min = 0;
    float dcc_distance = 0;

    uint64_t stop_time = 0;

    char GCODE_BUFFER[64];

    bool endstop_check_active = false;

    bool endstop_hit = false;


private:
    float direction_similarity(float x_mm, float y_mm, float x2_mm, float y2_mm);

public:

    void move_xy(float x_mm, float y_mm, float mm_min);
    void move_z(float mm, float mm_min);

    void set_xy_direction(float x_mm, float y_mm);
    void set_xy_speed(float mm_min);
    void raw_xy(float mm);

    #if SPINDEL_ADJUSTABEL
        void activate_spindle(float rpm){};
    #else
        void activate_spindle(bool on_off);
    #endif

    void endstop_check(bool on_off);
    static void endstop_callback(uint gpio, uint32_t events);

    void hard_stop();

    void reference();
    
    void run_programm();
    GCODE parse_GCODE(std::string GCODE_str);

    void Notfall_stop();
    void safety_stop();

public:

    CNC_Controler(Axis &x, Axis &y, Axis &z);

    ~CNC_Controler();

};


