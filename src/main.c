/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <sys/byteorder.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(main, 4);

static void start_scan(void);

static struct bt_conn *default_conn;

static bool decode_ad_data(struct bt_data *data, void *user_data) {
	const bt_addr_le_t *addr = user_data;
	if (data->type == 0x03 && data->data_len == 2 && 
			data->data[0] == 0x0a && data->data[1] == 0x18) { // Complete list of 16 bit service uuid type

		LOG_DBG("Found MEATER+. Stopping LE scan");
		if (bt_le_scan_stop()) {
			LOG_DBG("Failed to stop LE scan.");
			return false;
		}

		LOG_DBG("Attempting to connect...");

		int err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
			BT_LE_CONN_PARAM_DEFAULT, &default_conn);
		if (err) {
			LOG_DBG("Create conn failed (%u)\n", err);
			start_scan();
		}
		return false;
	}

	return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	char addr_str[BT_ADDR_LE_STR_LEN];
	// int err;

	if (default_conn) {
		return;
	}

	/* We're only interested in connectable events */
	if (type != BT_GAP_ADV_TYPE_ADV_IND &&
	    type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
		return;
	}

	bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

	bt_data_parse(ad, decode_ad_data, (void *)addr);
}

static void start_scan(void)
{
	int err;

	/* This demo doesn't require active scan */
	err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
	if (err) {
		LOG_DBG("Scanning failed to start (err %d)\n", err);
		return;
	}

	LOG_DBG("Scanning successfully started\n");
}

uint8_t temperature_notify_callback(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *data, uint16_t length) {
	if (!data) {
		LOG_DBG("Unsubscribed");
		return BT_GATT_ITER_STOP;
	}
	LOG_HEXDUMP_DBG(data, length, "Temperature data");

	uint8_t *bytes = data;

	uint16_t tip = bytes[0] | bytes[1] << 8;

	int celsius = (int)((float)tip + 8.0) / 16.0;

	LOG_DBG("Temperature in celsius: %d", celsius);

	return BT_GATT_ITER_CONTINUE;
}

static struct bt_gatt_discover_params discover_ccc = {0};
static struct bt_gatt_subscribe_params subscribe_params = {0};

static uint8_t discover_temperature_attribute_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params) {
	if (attr == NULL) {
		LOG_DBG("Discovery ended");
	} else {
		LOG_DBG("Discover temperature attribute: handle 0x%4X, perm 0x%4X", attr->handle, attr->perm);
		subscribe_params.notify = temperature_notify_callback,
		subscribe_params.value_handle = attr->handle + 1,
		subscribe_params.ccc_handle = 0,
		subscribe_params.end_handle = 0xffff,
		subscribe_params.disc_params = &discover_ccc,
		subscribe_params.value = BT_GATT_CCC_NOTIFY,

		bt_gatt_subscribe(conn, &subscribe_params);
	}

	return BT_GATT_ITER_STOP;
}

static struct bt_uuid_128 temperature_uuid = BT_UUID_INIT_128(
		BT_UUID_128_ENCODE(0x7edda774, 0x045e, 0x4bbf, 0x909b, 0x45d1991a2876));

static struct bt_uuid_128 primary_service = BT_UUID_INIT_128(
		BT_UUID_128_ENCODE(0xa75cc7fc, 0xc956, 0x488f, 0xac2a, 0x2dbc08b63a04));

static struct bt_gatt_discover_params discover_temperature = {
		.uuid = &temperature_uuid.uuid,
		.func = discover_temperature_attribute_cb,
		.type = BT_GATT_DISCOVER_CHARACTERISTIC,
		.start_handle = 0x0001,
		.end_handle = 0xffff,
};


// static uint8_t discover_battery_attribute(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params) {
// 	LOG_DBG("Discover battery attribute");
// }

static void connected(struct bt_conn *conn, uint8_t err)
{
	// char addr[BT_ADDR_LE_STR_LEN];

	// bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_DBG("Failed to connect (%u)\n", err);

		bt_conn_unref(default_conn);
		default_conn = NULL;

		start_scan();
		return;
	}

	if (conn != default_conn) {
		return;
	}

	LOG_DBG("Connected");

	err = bt_gatt_discover(conn, &discover_temperature);

	if (err) {
		LOG_ERR("Discover failed (err 0x%4X", err);
	}

	LOG_DBG("Discovering characteristics");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	if (conn != default_conn) {
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_DBG("Disconnected (reason 0x%02x)\n", reason);

	bt_conn_unref(default_conn);
	default_conn = NULL;

	start_scan();
}

static struct bt_conn_cb conn_callbacks = {
		.connected = connected,
		.disconnected = disconnected,
};

void main(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		LOG_DBG("Bluetooth init failed (err %d)\n", err);
		return;
	}

	LOG_DBG("Bluetooth initialized\n");

	bt_conn_cb_register(&conn_callbacks);

	start_scan();
}
