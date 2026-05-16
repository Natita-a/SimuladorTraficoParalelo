#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int main(void) {
    Config cfg = config_defaults();
    cfg = config_from_file(DEFAULT_CONFIG_PATH, cfg);
    config_print(&cfg, DEFAULT_CONFIG_PATH);
    return EXIT_SUCCESS;
}
