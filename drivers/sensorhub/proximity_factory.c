/*
 *  Copyright (C) 2012, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#include "ssp.h"

#define	VENDOR		"MAXIM"
#define	CHIP_ID		"MAX88005"

/*************************************************************************/
/* factory Sysfs                                                         */
/*************************************************************************/

static ssize_t prox_vendor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", VENDOR);
}

static ssize_t prox_name_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", CHIP_ID);
}

static ssize_t proximity_avg_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct ssp_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d, %d, %d\n",
		data->buf[PROXIMITY_RAW].prox[1],
		data->buf[PROXIMITY_RAW].prox[2],
		data->buf[PROXIMITY_RAW].prox[3]);
}

static ssize_t proximity_avg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	char chTempbuf[2] = { 1, 20};
	int iRet;
	int64_t dEnable;
	struct ssp_data *data = dev_get_drvdata(dev);

	iRet = strict_strtoll(buf, 10, &dEnable);
	if (iRet < 0)
		return iRet;

	if (dEnable) {
		send_instruction(data, ADD_SENSOR, PROXIMITY_RAW, chTempbuf, 2);
		data->bProximityRawEnabled = true;
	} else {
		send_instruction(data, REMOVE_SENSOR, PROXIMITY_RAW,
			chTempbuf, 2);
		data->bProximityRawEnabled = false;
	}

	return size;
}

static ssize_t proximity_state_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u8 uRowdata = 0;
	char chTempbuf[2] = { 1, 20};
	int iDelayCnt = 0;
	struct ssp_data *data = dev_get_drvdata(dev);

	if (data->bProximityRawEnabled == false) {
		send_instruction(data, ADD_SENSOR, PROXIMITY_RAW, chTempbuf, 2);
		uRowdata = data->buf[PROXIMITY_RAW].prox[0] = 0;

		while ((uRowdata == data->buf[PROXIMITY_RAW].prox[0])
			&& (iDelayCnt++ < 50))
			msleep(20);

		if ((iDelayCnt >= 50))
			pr_err("%s: [SSP] Prox Raw Timeout!!\n", __func__);

		uRowdata = data->buf[PROXIMITY_RAW].prox[0];

		send_instruction(data, REMOVE_SENSOR, PROXIMITY_RAW,
			chTempbuf, 2);
	} else
		uRowdata = data->buf[PROXIMITY_RAW].prox[0];

	return sprintf(buf, "%u\n", uRowdata);
}

static ssize_t proximity_cancel_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	bool bCalCMD;

	if (sysfs_streq(buf, "1")) /* calibrate cancelation value */
		bCalCMD = 1;
	else if (sysfs_streq(buf, "0")) /* reset cancelation value */
		bCalCMD = 0;
	else {
		pr_debug("%s: invalid value %d\n", __func__, *buf);
		return -EINVAL;
	}

	ssp_dbg("[SSP]: %s - %u\n", __func__, bCalCMD);
	return size;
}

static ssize_t proximity_cancel_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d,%d\n", 0, 0);
}

static ssize_t barcode_emul_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct ssp_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", data->bBarcodeEnabled);
}

static ssize_t barcode_emul_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	int iRet;
	int64_t dEnable;
	struct ssp_data *data = dev_get_drvdata(dev);

	iRet = strict_strtoll(buf, 10, &dEnable);
	if (iRet < 0)
		return iRet;

	if (dEnable)
		set_proximity_barcode_enable(data, 1);
	else
		set_proximity_barcode_enable(data, 0);

	return size;
}

static ssize_t gesture_rawdata_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct ssp_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d, %d, %d, %d\n",
		data->buf[GESTURE_SENSOR].data[0],
		data->buf[GESTURE_SENSOR].data[1],
		data->buf[GESTURE_SENSOR].data[2],
		data->buf[GESTURE_SENSOR].data[3]);
}

static DEVICE_ATTR(vendor, S_IRUGO, prox_vendor_show, NULL);
static DEVICE_ATTR(name, S_IRUGO, prox_name_show, NULL);
static DEVICE_ATTR(state, S_IRUGO, proximity_state_show, NULL);
static DEVICE_ATTR(gesture_rawdata, S_IRUGO, gesture_rawdata_show, NULL);
static DEVICE_ATTR(barcode_emul_en, S_IRUGO | S_IWUSR | S_IWGRP,
	barcode_emul_enable_show, barcode_emul_enable_store);
static DEVICE_ATTR(prox_avg, S_IRUGO | S_IWUSR | S_IWGRP,
	proximity_avg_show, proximity_avg_store);
static DEVICE_ATTR(prox_cal, S_IRUGO | S_IWUSR | S_IWGRP,
	proximity_cancel_show, proximity_cancel_store);

static struct device_attribute *prox_attrs[] = {
	&dev_attr_vendor,
	&dev_attr_name,
	&dev_attr_state,
	&dev_attr_prox_avg,
	&dev_attr_prox_cal,
	&dev_attr_barcode_emul_en,
	&dev_attr_gesture_rawdata,
	NULL,
};

void initialize_prox_factorytest(struct ssp_data *data)
{
	struct device *prox_device = NULL;

	sensors_register(prox_device, data, prox_attrs, "proximity_sensor");
}
