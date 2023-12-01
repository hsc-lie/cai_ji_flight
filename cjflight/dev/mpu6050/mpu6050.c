#include "mpu6050.h"



#define SAMPLE_RATE_DIVIDER               0x19        //陀螺仪采样频率寄存器地址
#define CONFIGURATION                     0x1a        //低通滤波频率
#define GYROSCOPE_CONFIGURATION           0x1b        //陀螺仪量程  0<<3 250度/s  1<<3 500度/s 2<<3 1000度/s 3<<3 2000度/s
#define ACCELEROMETER_CONFIGURATION       0x1c        //加速度量程 0<<3 2g   1<<3 4g   2<<3 8g   3<<3 16g
#define	ACCEL_XOUT_H	        0x3B
#define	ACCEL_XOUT_L	        0x3C
#define	ACCEL_YOUT_H	        0x3D
#define	ACCEL_YOUT_L	        0x3E
#define	ACCEL_ZOUT_H	        0x3F
#define	ACCEL_ZOUT_L	        0x40
#define	GYRO_XOUT_H				0x43
#define	GYRO_XOUT_L				0x44	
#define	GYRO_YOUT_H				0x45
#define	GYRO_YOUT_L				0x46
#define	GYRO_ZOUT_H				0x47
#define	GYRO_ZOUT_L				0x48
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define POWER_MANAGEMENT1       0x6b         //电源管理1
#define WHO_AM_I                0x75         //我是谁


static void mpu6050_delay()
{
	uint32_t i = 100000;
	while(--i);
}


E_MPU6050_ERROR MPU6050_Init(MPU6050_t * mpu6050)
{
	uint8_t writeData;
	uint8_t readData = 0xff;

	if((NULL == mpu6050)
		|| (NULL == mpu6050->I2CReadReg)
		|| (NULL == mpu6050->I2CWriteReg)
	)
	{
		return E_MPU6050_ERROR_NULL;
	}
	
	mpu6050_delay();
	
	//检测陀螺仪	
	while(readData != 0x98)
	//while(readData != 0x68)
	{	
		//mpu6050->I2CWriteReg(mpu6050->DevAddr, WHO_AM_I, &readData, 1);
		mpu6050->I2CReadReg(mpu6050->DevAddr, WHO_AM_I, &readData, 1);
		mpu6050_delay();
	}
	
		
	writeData = 0x00;
	mpu6050->I2CWriteReg(mpu6050->DevAddr, POWER_MANAGEMENT1, &writeData, 1);      //MPU6050电源管理

	writeData = 0x01;
	mpu6050->I2CWriteReg(mpu6050->DevAddr, SAMPLE_RATE_DIVIDER, &writeData, 1);   //陀螺仪采样频率

	writeData = 0x02;
	mpu6050->I2CWriteReg(mpu6050->DevAddr, CONFIGURATION, &writeData, 1);         //低通滤波 
		
	//陀螺仪量程  0<<3 250度/s  1<<3 500度/s 2<<3 1000度/s 3<<3 2000度/s
	writeData = mpu6050->GyroRange << 3;
	mpu6050->I2CWriteReg(mpu6050->DevAddr,GYROSCOPE_CONFIGURATION, &writeData, 1);
	
	//加速度量程 0<<3 2g   1<<3 4g   2<<3 8g   3<<3 16g
	writeData = mpu6050->AccRange << 3;
	mpu6050->I2CWriteReg(mpu6050->DevAddr,ACCELEROMETER_CONFIGURATION, &writeData, 1);
	
	return E_MPU6050_ERROR_OK;
}

//获得所有轴的角速度
E_MPU6050_ERROR MPU6050_GetBaseGyro(MPU6050_t * mpu6050, MPU6050_BaseData_t * gyro)
{
	uint8_t data[6] = {0};

	if((NULL == mpu6050)
		|| (NULL == mpu6050->I2CReadReg)
	)
	{
		return E_MPU6050_ERROR_NULL;
	}

	
	mpu6050->I2CReadReg(mpu6050->DevAddr, GYRO_XOUT_H, data, 6);

	gyro->X = (int16_t)((uint16_t)(data[0] << 8) | data[1]) - mpu6050->GyroZero.X;
	gyro->Y = (int16_t)((uint16_t)(data[2] << 8) | data[3]) - mpu6050->GyroZero.Y;
	gyro->Z = (int16_t)((uint16_t)(data[4] << 8) | data[5]) - mpu6050->GyroZero.Z;

	return E_MPU6050_ERROR_OK;
}


//获得所有轴的加速度
E_MPU6050_ERROR MPU6050_GetBaseAcc(MPU6050_t * mpu6050, MPU6050_BaseData_t * acc)
{
	uint8_t data[6] = {0};

	if((NULL == mpu6050)
		|| (NULL == mpu6050->I2CReadReg)
	)
	{
		return E_MPU6050_ERROR_NULL;
	}

	
	mpu6050->I2CReadReg(mpu6050->DevAddr, ACCEL_XOUT_H, data, 6);

	acc->X = (int16_t)((uint16_t)(data[0] << 8) | data[1]);
	acc->Y = (int16_t)((uint16_t)(data[2] << 8) | data[3]);
	acc->Z = (int16_t)((uint16_t)(data[4] << 8) | data[5]);

	return E_MPU6050_ERROR_OK;
}

//陀螺仪角速度单位转换
E_MPU6050_ERROR MPU6050_ConvertDataGyro(MPU6050_t * mpu6050, MPU6050_BaseData_t * in, MPU6050_ConvertData_t * out)
{
	float gyroConvertBase;

	if(NULL == mpu6050)
	{
		return E_MPU6050_ERROR_NULL;
	}

	gyroConvertBase = (float)(32768.0f / (250u << mpu6050->GyroRange));

	out->X = (float)in->X/gyroConvertBase;
	out->Y = (float)in->Y/gyroConvertBase;
	out->Z = (float)in->Z/gyroConvertBase;

	return E_MPU6050_ERROR_OK;
}

//加速度单位转换
E_MPU6050_ERROR MPU6050_ConvertDataAcc(MPU6050_t * mpu6050, MPU6050_BaseData_t * in, MPU6050_ConvertData_t * out)
{
	float accConvertBase;


	if(NULL == mpu6050)
	{
		return E_MPU6050_ERROR_NULL;
	}

	//accConvertBase = (float)(32768u >> (mpu6050->AccRange + 1));
	accConvertBase = 2048 / 0.98;

	out->X = (float)in->X/accConvertBase;
	out->Y = (float)in->Y/accConvertBase;
	out->Z = (float)in->Z/accConvertBase;

	return E_MPU6050_ERROR_OK;
}



