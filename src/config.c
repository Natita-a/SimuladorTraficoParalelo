#include "config.h"
#include <stdio.h>

Config config_defaults(void) {
    return (Config){
        .num_vehicles = DEFAULT_NUM_VEHICLES,
        .green_duration = DEFAULT_GREEN_DUR,
        .yellow_duration = DEFAULT_YELLOW_DUR,
        .red_duration = DEFAULT_RED_DUR,
    };
}

void config_print(const Config *cfg) {
    printf("[config] num_vehicles    : %d\n", cfg->num_vehicles);
    printf("[config] green_duration  : %ds\n", cfg->green_duration);
    printf("[config] yellow_duration : %ds\n", cfg->yellow_duration);
    printf("[config] red_duration    : %ds\n", cfg->red_duration);
}
