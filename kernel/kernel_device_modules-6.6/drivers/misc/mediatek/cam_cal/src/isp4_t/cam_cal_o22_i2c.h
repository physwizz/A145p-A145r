////@copyright:jianghongyan@huaqin.com
#ifndef __CAM_CAL_O22_I2C_H
#define __CAM_CAL_O22_I2C_H
#include <linux/i2c.h>

/************************************************************
 * 1.I2C read
 * 2.I2C write
 ************************************************************/
void iSetI2cClient(struct i2c_client *client);
int iReadRegI2C(u8 *a_pSendData, u16 a_sizeSendData,
		u8 *a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
int iWriteRegI2C(u8 *a_pSendData, u16 a_sizeSendData, u16 i2cId);

#endif				/* __CAM_CAL_O22_I2C_H */