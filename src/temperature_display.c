#include <temperature_display.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(temp_display, 4);

int temperature_display_init(void) {
    LOG_DBG("Temperature display init");

    return 0;
}

int temperature_display_set_inner_temperature(int inner_temperature) {
    LOG_DBG("Inner temperature: %d", inner_temperature);

    return 0;
}

int temperature_display_set_ambient_temperature(int ambient_temperature) {
    LOG_DBG("Ambient temperature: %d", ambient_temperature);

    return 0;
}

int temperature_display_set_unit(enum temperature_unit unit) {
    LOG_DBG("Temperature unit: %d", unit);

    return 0;
}