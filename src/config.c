#include "config.h"
#include "tomlc17.h"

#include <errno.h>
#include <stdio.h>

Config config_defaults(void) {
    return (Config){
        .num_vehicles = DEFAULT_NUM_VEHICLES,
        .green_duration = DEFAULT_GREEN_DUR,
        .yellow_duration = DEFAULT_YELLOW_DUR,
        .red_duration = DEFAULT_RED_DUR,
    };
}

Config config_from_file(const char *path, Config base) {
    toml_result_t result = toml_parse_file_ex(path);

    if (!result.ok) {
        /* Silent skip if the default config file simply doesn't exist */
        if (errno != ENOENT)
            fprintf(stderr, "warning: failed to parse '%s': %s\n", path,
                    result.errmsg);
        return base;
    }

    Config cfg = base;
    toml_datum_t v;

    /* [simulation] ----------------------------------------------------------
     */
    v = toml_seek(result.toptab, "simulation.num_vehicles");
    if (v.type == TOML_INT64)
        cfg.num_vehicles = (uint8_t)v.u.int64;

    /* [semaphore] -----------------------------------------------------------
     */
    v = toml_seek(result.toptab, "semaphore.green_duration");
    if (v.type == TOML_INT64)
        cfg.green_duration = (uint16_t)v.u.int64;

    v = toml_seek(result.toptab, "semaphore.yellow_duration");
    if (v.type == TOML_INT64)
        cfg.yellow_duration = (uint16_t)v.u.int64;

    v = toml_seek(result.toptab, "semaphore.red_duration");
    if (v.type == TOML_INT64)
        cfg.red_duration = (uint16_t)v.u.int64;

    toml_free(result);
    return cfg;
}

void config_print(const Config *cfg, const char *config_path) {
    printf("[config] source          : '%s' > defaults\n", config_path);
    printf("[config] num_vehicles    : %u\n", cfg->num_vehicles);
    printf("[config] green_duration  : %us\n", cfg->green_duration);
    printf("[config] yellow_duration : %us\n", cfg->yellow_duration);
    printf("[config] red_duration    : %us\n", cfg->red_duration);
}
