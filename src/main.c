#include <sys/util.h>
#include <logging/log.h>

#include <probe_connection.h>
#include <temperature_display.h>

LOG_MODULE_REGISTER(main, 4);

static int old_inner = INT32_MAX;
static int old_ambient = INT32_MAX;

void temperature_data_callback(const struct temperature_data_raw *data) {
	bool update_display = false;

	uint16_t inner = data->data[0] | data->data[1] << 8;
	int inner_celsius = (int)(((float)inner + 8.0) / 16.0);

	if (inner_celsius != old_inner) {
		temperature_display_set_inner_temperature(inner_celsius, 'C');
		old_inner = inner_celsius;
		update_display = true;
	}

	uint16_t r_ambient = data->data[2] | data->data[3] << 8;
	uint16_t o_ambient = data->data[4] | data->data[5] << 8;

	int ambient = (int)(inner + (MAX(0, ((((r_ambient - MIN(48, o_ambient)) * 16) * 589)) / 1487)));

	int ambient_celsius = (int)(((float)ambient + 8.0) / 16.0);

	if (ambient_celsius != old_ambient) {
		temperature_display_set_ambient_temperature(ambient_celsius, 'C');
		old_ambient = ambient_celsius;
		update_display = true;
	}

	if (update_display) {
		temperature_display_update();
	}

	LOG_DBG("Inner: %d, Outer: %d", inner_celsius, ambient_celsius);
}

void main(void)
{
	temperature_display_init();

	probe_connection_init(temperature_data_callback);
}