#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include <string>

#include "sd_card.h"
#include "ff.h"


void testing_sd();
void read_line(const char *path, char *buffer, size_t buffsize);