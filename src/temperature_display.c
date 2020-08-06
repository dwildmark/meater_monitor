#include <logging/log.h>
#include <errno.h>
#include <display/cfb.h>
#include <stdio.h>

#include <temperature_display.h>

LOG_MODULE_REGISTER(temp_display, 4);

static struct device *dev = NULL;

static char current_unit = 'C';
static char inner_temp_buffer[12] = {0};
static char ambient_temp_buffer[12] = {0};

static uint8_t ppt = 0;

int temperature_display_init(void) {
    LOG_DBG("Temperature display init");

    int err;

	dev = device_get_binding("SSD16XX");

	if (dev == NULL) {
		LOG_ERR("Device not found");
		return -ENODEV;
	}

	err = cfb_framebuffer_init(dev);
	if (err) {
		LOG_ERR("Framebuffer initialization failed!");
		return err;
	}

    err = cfb_framebuffer_set_font(dev, 2);
	if (err) {
		LOG_ERR("Framebuffer set font error=%d", err);
		return err;
	}

	LOG_DBG("Framebuffer initialized");

	ppt = cfb_get_display_parameter(dev, CFB_DISPLAY_PPT);

	err = cfb_framebuffer_clear(dev, true);
	if (err) {
		LOG_ERR("Framebuffer clear error=%d", err);
		return err;
	}

	err = cfb_framebuffer_finalize(dev);
	if (err) {
		LOG_ERR("Framebuffer finalize error=%d", err);
		return err;
	}

	return err;
}

int temperature_display_update(void) {
    cfb_framebuffer_clear(dev, false);

    cfb_print(dev, inner_temp_buffer, 0, 0);
    cfb_print(dev, ambient_temp_buffer, 0, 4 * ppt);

    cfb_framebuffer_finalize(dev);

    return 0;
}

int temperature_display_set_inner_temperature(int inner_temperature, char unit) {
    LOG_DBG("Inner temperature: %d", inner_temperature);

    snprintf(inner_temp_buffer, sizeof(inner_temp_buffer), "Inner:%3d %c", inner_temperature, unit);

    return 0;
}

int temperature_display_set_ambient_temperature(int ambient_temperature, char unit) {
    LOG_DBG("Ambient temperature: %d", ambient_temperature);

    snprintf(ambient_temp_buffer, sizeof(ambient_temp_buffer), "Outer:%3d %c", ambient_temperature, unit);

    return 0;
}
