#include "CNC_Controler.h"
#include <cstring>


// Die Methode parse_GCODE nimmt einen GCODE-String als Eingabe und gibt eine GCODE-Struktur zurück.
GCODE CNC_Controler::parse_GCODE(std::string GCODE_str){

    // Initialisierung der GCODE_BEFEHL und GCODE_PARAMETER Strukturen
    GCODE_BEFEHL Befehl{'F', 0};
    GCODE_PARAMETER Parameter = {std::nullopt, std::nullopt, std::nullopt, std::nullopt};

    // Überprüfen, ob der GCODE-String mit 'G', 'M' oder 'T' beginnt
    if (GCODE_str[0] != 'G' || GCODE_str[0] != 'M' || GCODE_str[0] != 'T')
        // Wenn nicht, wird ein Standard-GCODE zurückgegeben
        return {Befehl, Parameter};

    // Setzen des Typs des Befehls (G, M oder T)
    Befehl.Type = GCODE_str[0];

    // Extrahieren der Funktion des Befehls (die nächsten zwei Zeichen nach dem Typ) //Überprüfen was passiert wenn nicht M05 sondern M5
    Befehl.Funktion = std::stoi(std::string(&GCODE_str[1], 2));

    // TODO: Optimieren, damit nicht unnötig gesucht wird

    // Finden der Positionen der Parameter X, Y im GCODE-String
    uint8_t x_pos = GCODE_str.find('X');
    uint8_t y_pos = GCODE_str.find('Y');
        
    // Extrahieren und Konvertieren des X-Parameters, falls vorhanden
    if (x_pos != std::string::npos){
        uint8_t x_end = GCODE_str.find(' ', x_pos + 1);
        uint8_t y_end = GCODE_str.find(' ', y_pos + 1);

        if (x_end == std::string::npos)
            x_end = GCODE_str.length();
        
        if (y_end == std::string::npos)
            y_end = GCODE_str.length();

        std::string x_str = std::string(&GCODE_str[x_pos + 1], x_end);
        std::string y_str = std::string(&GCODE_str[y_pos + 1], y_end);

        Parameter.X = std::stof(x_str);
        Parameter.Y = std::stof(y_str);
        return {Befehl, Parameter};
                
    }


    // Extrahieren und Konvertieren des Z-Parameters, falls vorhanden
    uint8_t z_pos = GCODE_str.find('Z');

    if (z_pos != std::string::npos){
        uint8_t z_end = GCODE_str.find(' ', z_pos + 1);
        if (z_end == std::string::npos)
            z_end = GCODE_str.length();

        std::string z_str = std::string(&GCODE_str[z_pos + 1], z_end);
        Parameter.Z = std::stof(z_str);

        return {Befehl, Parameter};
    }

    uint8_t f_pos = GCODE_str.find('F');

    // Extrahieren und Konvertieren des F-Parameters, falls vorhanden
    if (f_pos != std::string::npos){
        uint8_t f_end = GCODE_str.find(' ', f_pos + 1);
        if (f_end == std::string::npos)
            f_end = GCODE_str.length();

        std::string f_str = std::string(&GCODE_str[f_pos + 1], f_end);
        Parameter.F = std::stof(f_str);
        return {Befehl, Parameter};
    }


    #if SPINDEL_ADJUSTABEL
    uint8_t s_pos = GCODE_str.find('S');
    
    if (s_pos != std::string::npos){
        uint8_t s_end = GCODE_str.find(' ', s_pos + 1);
        if (s_end == std::string::npos)
            s_end = GCODE_str.length();

        std::string s_str = std::string(&GCODE_str[s_pos + 1], s_end);
        Parameter.S = std::stof(s_str);

        return {Befehl, Parameter};
    }
    #endif
    // Rückgabe der GCODE-Struktur mit Befehl und Parametern
    return {Befehl, Parameter};
}
