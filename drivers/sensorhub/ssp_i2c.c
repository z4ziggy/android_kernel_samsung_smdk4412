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

#define PRX_THRSH_HIGH_PARAM	150
#define PRX_THRSH_LOW_PARAM	90

static bool wait_mcu_wakeup(struct ssp_data *data)
{
	int iDelaycnt = 0;

	data->wakeup_mcu();
	while ((!data->check_mcu_ready()) && (iDelaycnt++ < 1000))
		udelay(10);

	if (iDelaycnt >= 1000) {
		pr_err("%s: [SSP] MCU Wakeup Timeout!!\n", __func__);
		return false;
	}

	return true;
}

static int ssp_i2c_read(struct ssp_data *data, char *pTxData, u16 uTxLength,
	char *pRxData, u16 uRxLength)
{
	struct i2c_client *client = data->client;
	struct i2c_msg msgs[] = {
		{
		 .addr = client->addr,
		 .flags = 0,
		 .len = uTxLength,
		 .buf = pTxData,
		},
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = uRxLength,
		 .buf = pRxData,
		},
	};

	if (i2c_transfer(client->adapter, msgs, 2) < 0) {
		pr_err("%s: [SSP] i2c transfer error\n", __func__);
		return -EIO;
	} else
		return true;
}

int ssp_sleep_mode(struct ssp_data *data)
{
	char chRxData = 0;
	char chTxData = MSG2SSP_AP_STATUS_SLEEP;
	int iRet;

	if (wait_mcu_wakeup(data) == false)
		return -1;

	/* send to AP_STATUS_SLEEP */
	iRet = ssp_i2c_read(data, &chTxData, 1, &chRxData, 1);
	if ((chRxData != MSG_ACK) || (iRet != true)) {
		pr_err("%s: [SSP] MSG2SSP_AP_STATUS_SLEEP CMD fail\n",
				__func__);
		return false;
	}
	ssp_dbg("%s: [SSP] MSG2SSP_AP_STATUS_SLEEP CMD\n", __func__);

	return true;
}

int ssp_resume_mode(struct ssp_data *data)
{
	int iRet;
	char chRxData = 0;
	char chTxData = MSG2SSP_AP_STATUS_WAKEUP;

	if (wait_mcu_wakeup(data) == false)
		return -1;

	/* send to AP_STATUS_WAKEUP */
	iRet = ssp_i2c_read(data, &chTxData, 1, &chRxData, 1);
	if ((chRxData != MSG_ACK) || (iRet != true)) {
		pr_err("%s: [SSP] MSG2SSP_AP_STATUS_WAKEUP CMD fail\n",
		       __func__);
		return false;
	}
	ssp_dbg("%s: [SSP] MSG2SSP_AP_STATUS_WAKEUP CMD\n", __func__);

	return true;
}

int send_instruction(struct ssp_data *data, u8 uInst,
	u8 uSensorType, u8 *uSendBuf, u8 uLength)
{
	char aTxData[uLength + 5];
	char chRxbuf = 0;
	int iRet = 0;

	if (wait_mcu_wakeup(data) == false)
		return -1;

	aTxData[0] = MSG2SSP_STS;
	aTxData[1] = 3 + uLength;
	aTxData[2] = MSG2SSP_SSM;

	switch (uInst) {
	case REMOVE_SENSOR:
		aTxData[3] = MSG2SSP_INST_BYPASS_SENSOR_REMOVE;
		break;
	case ADD_SENSOR:
		aTxData[3] = MSG2SSP_INST_BYPASS_SENSOR_ADD;
		break;
	case CHANGE_DELAY:
		aTxData[3] = MSG2SSP_INST_CHANGE_DELAY;
		break;
	case GO_SLEEP:
		aTxData[3] = MSG2SSP_AP_STATUS_SLEEP;
		break;
	case FACTORY_MODE:
		aTxData[3] = MSG2SSP_INST_SENSOR_SELFTEST;
		break;
	default:
		return false;
	}

	aTxData[4] = uSensorType;
	memcpy(&aTxData[5], uSendBuf, uLength);

	ssp_dbg("%s: Inst = 0x%x, Sensor Type = 0x%x, data = %u, %u\n",
		 __func__, aTxData[3], aTxData[4], aTxData[5], aTxData[6]);

	iRet = ssp_i2c_read(data, aTxData, 2, &chRxbuf, 1);
	if ((chRxbuf != MSG_ACK) || (iRet != true)) {
		pr_err("%s: [SSP] MSG2SSP_STS - Fail to send instruction\n",
		       __func__);
		return false;
	} else {
		chRxbuf = 0;
		iRet = ssp_i2c_read(data, &(aTxData[2]),
			(u16)aTxData[1], &chRxbuf, 1);
		if ((chRxbuf != MSG_ACK) || (iRet != true)) {
			pr_err("%s: [SSP] Instruction CMD Fail\n", __func__);
			return false;
		}
		return true;
	}
}

static int ssp_recieve_msg(struct ssp_data *data,  u8 uLength)
{
	char chTxBuf = 0;
	char *pchRcvDataFrame;	/* SSP-AP Massage data buffer */
	int iRet = 0;

	if (uLength > 0) {
		pchRcvDataFrame = kzalloc((uLength * sizeof(char)), GFP_KERNEL);
		chTxBuf = MSG2SSP_SRM;
		iRet = ssp_i2c_read(data, &chTxBuf, 1, pchRcvDataFrame,
				(u16)uLength);
		if (iRet != true) {
			pr_err("%s: [SSP] Fail to recieve SSPdata\n", __func__);
			kfree(pchRcvDataFrame);
			return false;
		}
	} else {
		pr_err("%s: [SSP] No ready data. length = %d\n",
			__func__, uLength);
		return false;
	}

	parse_dataframe(data, pchRcvDataFrame, uLength);

	kfree(pchRcvDataFrame);
	return uLength;
}

void set_sensor_position(struct ssp_data *data)
{
	char chTxBuf[4] = { 0, };
	char chRxData;
	int iRet;

	chTxBuf[0] = MSG2SSP_AP_SENSOR_FORMATION;

	chTxBuf[1] = CONFIG_SENSORS_SSP_ACCELEROMETER_POSITION;
	chTxBuf[2] = CONFIG_SENSORS_SSP_GYROSCOPE_POSITION;
	chTxBuf[3] = CONFIG_SENSORS_SSP_MAGNETOMETER_POSITION;

#if defined(CONFIG_MACH_T0_EUR_OPEN)
	if (data->check_ap_rev() == 0x03) {
		chTxBuf[1] = 7;
		chTxBuf[2] = 7;
		chTxBuf[3] = CONFIG_SENSORS_SSP_MAGNETOMETER_POSITION;
	}
#endif

#if defined(CONFIG_MACH_T0_USA_SPR) || defined(CONFIG_MACH_T0_USA_USCC)\
	|| defined(CONFIG_MACH_T0_USA_VZW)
	chTxBuf[1] = 4;
	chTxBuf[2] = 4;
	chTxBuf[3] = CONFIG_SENSORS_SSP_MAGNETOMETER_POSITION;
#endif

	pr_info("[SSP] Sensor Posision A : %u, G : %u, M: %u\n",
		chTxBuf[1], chTxBuf[2], chTxBuf[3]);

	iRet = ssp_i2c_read(data, chTxBuf, 4, &chRxData, 1);
	if ((chRxData != MSG_ACK) || (iRet != true))
		pr_err("%s: [SSP] - i2c fail %d\n", __func__, iRet);
}

void set_proximity_threshold(struct ssp_data *data)
{
	char chTxBuf[3] = { 0, };
	char chRxData;
	int iRet;

	chTxBuf[0] = MSG2SSP_AP_SENSOR_PROXTHRESHOLD;
	chTxBuf[1] = PRX_THRSH_HIGH_PARAM;
	chTxBuf[2] = PRX_THRSH_LOW_PARAM;

	pr_info("[SSP] Proximity Threshold HIGH : %d, LOW : %d\n",
		chTxBuf[1], chTxBuf[2]);

	iRet = ssp_i2c_read(data, chTxBuf, 3, &chRxData, 1);
	if ((chRxData != MSG_ACK) || (iRet != true))
		pr_err("%s: [SSP] - i2c fail %d\n", __func__, iRet);
}

void set_proximity_barcode_enable(struct ssp_data *data, bool bEnable)
{
	char chTxBuf[2] = { 0, };
	char chRxData;
	int iRet;

	chTxBuf[0] = MSG2SSP_AP_SENSOR_BARCODE_EMUL;
	chTxBuf[1] = bEnable;

	data->bBarcodeEnabled = bEnable;
	pr_info("[SSP] Proximity Barcode En : %u\n", bEnable);

	iRet = ssp_i2c_read(data, chTxBuf, 2, &chRxData, 1);
	if ((chRxData != MSG_ACK) || (iRet != true))
		pr_err("%s: [SSP] - i2c fail %d\n", __func__, iRet);
}

unsigned int get_sensor_scanning_info(struct ssp_data *data)
{
	char chTxBuf = MSG2SSP_AP_SENSOR_SCANNING;
	char chRxData[2] = {0,};
	int iRet;

	iRet = ssp_i2c_read(data, &chTxBuf, 1, chRxData, 2);
	if (iRet != true)
		pr_err("%s: [SSP] - i2c fail %d\n", __func__, iRet);

	return ((unsigned int)chRxData[0] << 8) | chRxData[1];
}

unsigned int get_firmware_rev(struct ssp_data *data)
{
	char chTxData = MSG2SSP_AP_FIRMWARE_REV;
	char chRxBuf[3] = { 0, };
	int iRet;

	iRet = ssp_i2c_read(data, &chTxData, 1, chRxBuf, 3);
	if (iRet != true) {
		pr_err("%s: [SSP] - i2c fail %d\n", __func__, iRet);
		return (unsigned int)99999999;
	}

	return (chRxBuf[0] << 16) | (chRxBuf[1] << 8) | chRxBuf[2];
}

int get_fuserom_data(struct ssp_data *data)
{
	char chTxBuf[2] = { 0, };
	char chRxBuf[2] = { 0, };
	int iRet = 0;
	unsigned int uLength = 0;

	chTxBuf[0] = MSG2SSP_AP_STT;
	chTxBuf[1] = MSG2SSP_AP_FUSEROM;

	/* chRxBuf is Two Byte because msg is large */
	iRet = ssp_i2c_read(data, chTxBuf, 2, chRxBuf, 2);
	uLength = ((unsigned int)chRxBuf[0] << 8) + (unsigned int)chRxBuf[1];

	if (iRet != true) {
		pr_err("%s: [SSP] MSG2SSP_AP_STT - i2c fail %d\n",
				__func__, iRet);
		goto err_read_fuserom;
	} else if (uLength <= 0) {
		pr_err("%s: [SSP] No ready data. length = %u\n",
				__func__, uLength);
		goto err_read_fuserom;
	} else {
		if (data->iLibraryLength != 0)
			kfree(data->pchLibraryBuf);

		data->iLibraryLength = (int)uLength;
		data->pchLibraryBuf =
		    kzalloc((data->iLibraryLength * sizeof(char)), GFP_KERNEL);

		chTxBuf[0] = MSG2SSP_SRM;
		iRet = ssp_i2c_read(data, chTxBuf, 1, data->pchLibraryBuf,
			(u16)data->iLibraryLength);
		if (iRet != true) {
			pr_err("%s: [SSP] Fail to recieve SSP data\n",
					__func__);
			kfree(data->pchLibraryBuf);
			data->iLibraryLength = 0;
			goto err_read_fuserom;
		}
	}

	data->uFuseRomData[0] = data->pchLibraryBuf[0];
	data->uFuseRomData[1] = data->pchLibraryBuf[1];
	data->uFuseRomData[2] = data->pchLibraryBuf[2];

	pr_info("[SSP] FUSE ROM Data %d , %d, %d\n", data->uFuseRomData[0],
		data->uFuseRomData[1], data->uFuseRomData[2]);

	data->iLibraryLength = 0;
	kfree(data->pchLibraryBuf);
	return true;

err_read_fuserom:
	data->uFuseRomData[0] = 0;
	data->uFuseRomData[1] = 0;
	data->uFuseRomData[2] = 0;

	return false;
}

int select_irq_msg(struct ssp_data *data)
{
	u8 chLength = 0;
	char chTxBuf = 0;
	char chRxBuf[2] = { 0, };
	int iRet = 0;

	chTxBuf = MSG2SSP_SSD;
	iRet = ssp_i2c_read(data, &chTxBuf, 1, chRxBuf, 2);

	if (iRet != true) {
		pr_err("%s: [SSP] MSG2SSP_RTS - i2c fail %d\n",
			__func__, iRet);
		return iRet;
	} else {
		if (chRxBuf[0] == MSG2SSP_RTS) {
			chLength = (u8)chRxBuf[1];
			ssp_recieve_msg(data, chLength);
		}
#ifdef CONFIG_SENSORS_SSP_SENSORHUB
		else if (chRxBuf[0] == MSG2SSP_STT) {
			pr_err("%s: MSG2SSP_STT irq", __func__);
			iRet = ssp_receive_large_msg(data, (u8)chRxBuf[1]);
			if (iRet >= 0) {
				ssp_report_context_data(data);
				wake_lock_timeout(&data->sensorhub_wake_lock,
						2 * HZ);
			} else {
				pr_err("%s: receive large msg err(%d)",
					__func__, iRet);
			}
		}
#endif
	}
	return true;
}
