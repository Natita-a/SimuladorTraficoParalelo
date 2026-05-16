#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// --- Hardcoded defaults ------------------------------------------------------

#define DEFAULT_NUM_VEHICLES ((uint8_t)20)
#define DEFAULT_GREEN_DUR ((uint16_t)5)
#define DEFAULT_YELLOW_DUR ((uint16_t)1)
#define DEFAULT_RED_DUR ((uint16_t)4)
#define MAX_VEHICLES ((uint8_t)200)

// --- Config ------------------------------------------------------------------

typedef struct {
    uint8_t num_vehicles;
    uint16_t green_duration;
    uint16_t yellow_duration;
    uint16_t red_duration;
} Config;

// --- API ---------------------------------------------------------------------

Config config_defaults(void);
void config_print(const Config *cfg);

#endif /* CONFIG_H */
