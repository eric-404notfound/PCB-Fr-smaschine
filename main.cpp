// Last update: 2021-09-29 20:00:00
// Serial Host: indexnull.de Port: 3382
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "CNC_Controler.h"
#include "Files.h"


#include "lcd.h"

Axis x(190, 14, 15, 'x', 7, 6); // X-Achse: Länge 190mm, Step-Pin 14, Dir-Pin 15, Achsenname 'x', Endstop-Pin Plus 7, Minus 6
Axis y(290, 16, 17, 'y', 9, 8); // Y-Achse: Länge 290mm, Step-Pin 16, Dir-Pin 17, Achsenname 'y', Endstop-Pin Plus 9, Minus 8
Axis z(40, 18, 19, 'z', 11, 10); // Z-Achse: Länge 40mm, Step-Pin 18, Dir-Pin 19, Achsenname 'z', Endstop-Pin Plus 11, Minus 10
CNC_Controler cnc(x, y, z); // CNC-Controller-Instanz mit den Achsen X, Y und Z


// Noch nicht perfektioniert aber die Grundfunktionen sind da (zurzeit noch ohne Spindelsteuerung)
int main() {

    stdio_init_all();
    sleep_ms(1000);
    for (int i = 0; i < 100; i++)
    {
        printf("\n");
    }
    
    printf("Hallo welt\n");
    

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    cnc.reference();

    cnc.set_abs_mode(false);
    cnc.move_xy(-160, -75, 700);
    cnc.move_z(0,300);
    cnc.set_pos(20,100,-20);
    cnc.set_abs_mode(true);
    init_sd();
    cnc.run_programm();

}
