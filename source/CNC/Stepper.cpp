// Created: 2021-04-17 16:00:00
#include "Stepper.h"

uint8_t Stepper::used_axis = 0;

// Bewegung direkt ausführen (ohne DMA)
void Stepper::raw_move(uint32_t value){

    #if DEBUG > 2
    uint32_t time = time_us_32();
    printf("axis:%c ", char(this->axis + 0x57));
    #endif
        
    while (pio_sm_is_tx_fifo_full(this->pio, this->sm))
        sleep_ms(500);
    

    #if DEBUG > 2
    printf("Moved / Blocked for %f \n", time_us_32() - time);  
    #endif

    pio_sm_put_blocking(this->pio, this->sm, value);

};

// Bewegung über DMA oder direkt je nach Konfiguration
void Stepper::move(uint32_t value){

    #ifdef DMA_ENABLED

    this->stepper_n++;

    if(!dma_channel_is_busy(this->dma_chan)){
        if(pio_sm_get_tx_fifo_level(this->pio, this->sm) == 0){
            dma_channel_transfer_from_buffer_now(this->dma_chan, this->buffer[this->buffer_index], this->buffer_offset);
            this->dma_n++;
            this->buffer_index ^= 1;
            this->buffer_offset = 0;
        }
    }

    if(this->buffer_offset >= DMA_BUFFER_SIZE){
        while(dma_channel_is_busy(this->dma_chan))
            tight_loop_contents();

        while (pio_sm_get_tx_fifo_level(this->pio, this->sm) != 0)
            tight_loop_contents();
 
        dma_channel_transfer_from_buffer_now(this->dma_chan, this->buffer[this->buffer_index], this->buffer_offset);
        this->dma_n++;

        this->buffer_index ^= 1;
        this->buffer_offset = 0;

    }

    buffer[this->buffer_index][this->buffer_offset++] = value;

    printf("Axis:%c dma_n: %d / stepper_n: %d \n",char(this->axis + 0x57) ,this->dma_n, this->stepper_n);
    #else
        raw_move(value);
    #endif

    
};


#ifdef DMA_ENABLED
// Einrichtung des DMA-Kanals für den Stepper
void Stepper::setup_dma(){

    this->dma_chan = dma_claim_unused_channel(true); // Claim a DMA channel
    this->dma_config = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&this->dma_config, DMA_SIZE_32);
    channel_config_set_read_increment(&this->dma_config, true);
    uint dreq = pio_get_dreq(this->pio, this->sm, true);
    channel_config_set_dreq(&this->dma_config, dreq);

    dma_channel_configure(
        dma_chan,
        &this->dma_config,
        &this->pio->txf[this->sm],    // Write address
        nullptr,                      // Read address
        DMA_BUFFER_SIZE,              // Number of transfers (0 = unlimited) 
        false                         // Don't start
    );
};

// Prüfen, ob DMA abgeschlossen ist
bool Stepper::dma_finish(){

    if(!dma_channel_is_busy(this->dma_chan))
        return false;

    if(pio_sm_get_tx_fifo_level(this->pio, this->sm) > 0)
        return false;

    if(this->buffer_offset > 0){
        dma_channel_transfer_from_buffer_now(this->dma_chan, this->buffer[this->buffer_index], this->buffer_offset);
        this->dma_n++;
        this->buffer_index ^= 1;
        this->buffer_offset = 0;
        return false;
    }

    return true;
}
#endif

// Prüfen ob gerade Schritte ausgeführt werden
bool Stepper::processing(){

    #ifdef DMA_ENABLED
    return dma_finish();
    #endif
    return !pio_sm_is_tx_fifo_empty(this->pio, this->sm);
    
}
// Sofortiger Stopp der Bewegung (auch PIO-Reset)
void Stepper::stop(){
    #ifdef DMA_ENABLED
        dma_channel_abort(this->dma_chan);
        #warning "DMA nicht überdacht"
    #endif

    pio_sm_set_enabled(this->pio, this->sm, false);  // SM anhalten
    pio_sm_restart(this->pio, this->sm);             // Reset inkl. Program Counter
    pio_sm_clear_fifos(this->pio, this->sm);         // FIFO leeren
    pio_sm_set_enabled(this->pio, this->sm, true);
    pio_sm_exec(this->pio, this->sm, pio_encode_jmp(this->offset));


}

// Konstruktor für Stepper mit initialer PIO- und SM-Zuweisung je nach Achse
Stepper::Stepper(uint8_t step_pin, uint8_t dir_pin, char axis){

    this->step_pin = step_pin;
    this->dir_pin = dir_pin;

    offset;
    switch (axis){
    case 'x':

        if(used_axis & 0x1)
            return;

        this->used_axis |= 0x1;
        this->axis = 0x1;

        this->pio = pio0;
        this->sm = pio_claim_unused_sm(this->pio, true);

        gpio_set_function(this->step_pin, GPIO_FUNC_PIO0);
        gpio_set_function(this->dir_pin, GPIO_FUNC_PIO0);

        offset = pio_add_program(this->pio, &stepper_x_program);
        stepper_x_program_init(this->pio, this->sm, offset, this->step_pin, this->dir_pin, MICROSTEPS);
        break;
        
    case 'y':
        if(this->used_axis & 0x2)
            return;
        this->used_axis |= 0x2;
        this->axis = 0x2;

        this->pio = pio0;
        this->sm = pio_claim_unused_sm(this->pio, true);

        gpio_set_function(this->step_pin, GPIO_FUNC_PIO0);
        gpio_set_function(this->dir_pin, GPIO_FUNC_PIO0);

        offset = pio_add_program(this->pio, &stepper_y_program);
        stepper_y_program_init(this->pio, this->sm, offset, this->step_pin, this->dir_pin, MICROSTEPS);
        break;

    case 'z':
        if(used_axis & 0x4)
            return;
        this->used_axis |= 0x4;
        this->axis = 0x4;

        this->pio = pio1;
        this->sm = pio_claim_unused_sm(this->pio, true);

        gpio_set_function(this->step_pin, GPIO_FUNC_PIO1);
        gpio_set_function(this->dir_pin, GPIO_FUNC_PIO1);

        offset = pio_add_program(this->pio, &stepper_z_program);
        stepper_z_program_init(this->pio, this->sm, offset, this->step_pin, this->dir_pin, MICROSTEPS);
        break;
    
    default:
        printf("Axis not supported\n");
    }
    
    #ifdef DMA_ENABLED
    setup_dma();
    #endif
}

// Destruktor: Freigabe der genutzten Ressourcen
Stepper::~Stepper(){

    pio_sm_unclaim(this->pio, this->sm);

    #ifdef DMA_ENABLED
        dma_channel_unclaim(this->dma_chan);
    #endif

    used_axis ^= this->axis;
}

