#include "Files.h"


FATFS fs;
FRESULT fr;


char buffer[128];
char filename[128]  = "0:/";


std::string pathstring = "0:/";


void init_sd(){
    if(!sd_init_driver()){
        printf("SD card not found\n");
        return;
    }

    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("f_mount error (%d)\n", fr);
        return;
    }
    printf("SD card mounted 0:\n");
}

void list_dir(const char* path){

    DIR dir;
    FILINFO fno;
    FIL file;
    UINT br;


    fr = f_opendir(&dir, path);
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
            printf("File:%s\n", fno.fname);
        }
    }
    f_closedir(&dir);
}

void write_file(const char* path, const char* content, int length){

    DIR dir;
    FILINFO fno;
    UINT br;
    FIL file;

    fr = f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("f_open error (%d)\n", fr);
        return;
    }
    fr = f_write(&file, content, length, &br);
    if (fr != FR_OK) {
        printf("f_write error (%d)\n", fr);
        return;
    }
    f_close(&file);
}

void testing_sd(){
    init_sd();
    list_dir("0:/");
    list_dir("0:/Neuer Ordner");
    write_file("Versuch1.txt", "Hello World\n", 12);
}