#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int main(void) {
    Config cfg = config_defaults();
    config_print(&cfg);
    return EXIT_SUCCESS;
}
