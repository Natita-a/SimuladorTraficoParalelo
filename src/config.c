#include "config.h"
#include "tomlc17.h"

#include <errno.h>
#include <stdio.h>

static uint16_t clamp_u16(long long v, uint16_t fallback) {
    if (v <= 0)
        return fallback;
    if (v > UINT16_MAX)
        return UINT16_MAX;
    return (uint16_t)v;
}

static uint8_t clamp_u8(long long v, uint8_t fallback, uint8_t max_value) {
    if (v <= 0)
        return fallback;
    if (v > max_value)
        return max_value;
    return (uint8_t)v;
}

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
        if (errno != ENOENT) {
            fprintf(stderr, "warning: failed to parse '%s': %s\n", path,
                    result.errmsg ? result.errmsg : "unknown error");
        }
        return base;
    }

    Config cfg = base;
    toml_datum_t v;

    v = toml_seek(result.toptab, "simulation.num_vehicles");
    if (v.type == TOML_INT64)
        cfg.num_vehicles = clamp_u8(v.u.int64, base.num_vehicles, MAX_VEHICLES);

    v = toml_seek(result.toptab, "semaphore.green_duration");
    if (v.type == TOML_INT64)
        cfg.green_duration = clamp_u16(v.u.int64, base.green_duration);

    v = toml_seek(result.toptab, "semaphore.yellow_duration");
    if (v.type == TOML_INT64)
        cfg.yellow_duration = clamp_u16(v.u.int64, base.yellow_duration);

    v = toml_seek(result.toptab, "semaphore.red_duration");
    if (v.type == TOML_INT64)
        cfg.red_duration = clamp_u16(v.u.int64, base.red_duration);

    toml_free(result);
    return cfg;
}

void config_print(const Config *cfg, const char *config_path) {
    printf("[config] source          : %s\n",
           config_path ? config_path : "(defaults)");
    printf("[config] num_vehicles    : %u\n", cfg->num_vehicles);
    printf("[config] green_duration  : %us\n", cfg->green_duration);
    printf("[config] yellow_duration : %us\n", cfg->yellow_duration);
    printf("[config] red_duration    : %us\n", cfg->red_duration);
}
