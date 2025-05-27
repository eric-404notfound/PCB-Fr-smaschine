// Last update: 2021-09-29 20:00:00
// Serial Host: indexnull.de Port: 3382
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "CNC_Controler.h"
#include "Files.h"


#include "lcd.h"

Axis x(190, 14, 15, 'x', 7, 6);
Axis y(290, 16, 17, 'y', 9, 8);
Axis z(40, 18, 19, 'z', 11, 10);
CNC_Controler cnc(x, y, z);


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
    

    int x_1 = 0;
    int y_1 = 0;
    while (z.get_endstop_minus())
        tight_loop_contents();

    gpio_put(PICO_DEFAULT_LED_PIN, 1);  
    while (z.get_endstop_plus())
        tight_loop_contents();
    //testing_sd();

    cnc.reference();
    cnc.endstop_check(true);
    cnc.move_xy(-40,-40,300);
    cnc.set_abs_mode(true);
    cnc.move_xy(10,20,300);
    cnc.move_xy(100,200,300);
    cnc.move_xy(10,20,300);
    cnc.move_xy(100,200,300);
    cnc.set_abs_mode(false);
    cnc.move_xy(10,20,300);
    cnc.move_xy(100,200,300);
    cnc.move_xy(10,20,300);
    cnc.move_xy(100,200,300);
    

    return 1;
    cnc.run_programm();

    return 0;
    while (1)    {

        cnc.move_xy(100,100,500);
        cnc.move_xy(-100, -100, 500);

        continue;
        printf("X: ");
        scanf("%d", &x_1);
        printf("%d\n", x_1);
        printf("Y: ");
        scanf("%d", &y_1);
        printf("%d\n", y_1);
        
        cnc.move_xy(x_1, y_1, 500);
       // cnc.move_z(10,100);

    }

}
