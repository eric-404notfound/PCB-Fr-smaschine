// Last update: 2021-09-29 20:00:00
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "CNC_Controler.h"

#include "lcd.h"

Axis x(190, 14, 15, 'x', 4, 4);
Axis y(290, 16, 17, 'y', 5, 5);
Axis z(40, 18, 19, 'z', 6, 6);
CNC_Controler cnc(x, y, z);

int main() {

    stdio_init_all();
    sleep_ms(1000);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    int x_1 = 0;
    int y_1 = 0;

    gpio_init(2);
    gpio_set_dir(2 , GPIO_IN);
    gpio_pull_up(2);
    
    
    testing_lcd();
    

    return 0;
    cnc.move_z(10,100); 
    cnc.move_xy(-40,0,700); //750 max
    cnc.reference();

    while (1)
    {
        printf("State: %s\n", gpio_get(2) ? "HIGH" : "LOW");   
        printf("X: ");
        scanf("%d", &x_1);
        printf("Y: ");
        scanf("%d", &y_1);

        printf("X:%d Y:%d\n", x_1, y_1);
        
        cnc.move_xy(x_1, y_1, 500);
       // cnc.move_z(10,100);

    }

}