/**
 * Copyright 2020 Dennis Wildmark
 */

#ifndef TEMPERATURE_DISPLAY_H
#define TEMPERATURE_DISPLAY_H

int temperature_display_init(void);

int temperature_display_set_inner_temperature(int inner_temperature);

int temperature_display_set_ambient_temperature(int ambient_temperature);

enum temperature_unit {
    UNIT_CELSIUS,
    UNIT_FARENHEIT,
    NUM_UNITS
};

int temperature_display_set_unit(enum temperature_unit unit);

#endif /* TEMPERATURE_DISPLAY_H */
