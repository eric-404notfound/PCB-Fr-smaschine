
#pragma once
#include <hardware/pio.h>
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pio_stepper.pio.h"
#include "pico/time.h"
#include <stdio.h>

#define MICROSTEPS 8
#define DMA_BUFFER_SIZE 32

//#define DMA_ENABLED


class Stepper{
private:

    uint8_t step_pin;
    uint8_t dir_pin;

    
    static uint8_t used_axis;
    uint8_t axis;

    #ifdef DMA_ENABLED

    int dma_chan;
    dma_channel_config dma_config;

    size_t buffer_index = 0;
    size_t buffer_offset = 0;
    uint32_t buffer[2][DMA_BUFFER_SIZE];

    uint32_t dma_n;
    uint32_t stepper_n;

    #endif
    
    uint sm;
    pio_hw_t *pio;

private:

    #ifdef DMA_ENABLED
    void setup_dma();
    bool dma_finish();
    #endif

    void raw_move(uint32_t value);
    
public:

public:

    Stepper(uint8_t step_pin, uint8_t dir_pin, char axis);
    ~Stepper();

    bool processing();
    void move(uint32_t value);

    void stop();
    void start();


};
