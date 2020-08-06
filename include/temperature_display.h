/**
 * Copyright 2020 Dennis Wildmark
 */

#ifndef TEMPERATURE_DISPLAY_H
#define TEMPERATURE_DISPLAY_H

int temperature_display_init(void);

int temperature_display_update(void);

int temperature_display_set_inner_temperature(int inner_temperature, char unit);

int temperature_display_set_ambient_temperature(int ambient_temperature, char unit);

#endif /* TEMPERATURE_DISPLAY_H */
