/**
 * Copyright 2020 Dennis Wildmark
 */

#ifndef PROBE_CONNECTION_H 
#define PROBE_CONNECTION_H

#include <stdint.h>

struct temperature_data_raw {
    uint8_t data[8];
};

typedef void (*temperature_data_callback_t)(const struct temperature_data_raw *data);

int probe_connection_init(temperature_data_callback_t temperature_cb);

#endif /* PROBE_CONNECTION_H */