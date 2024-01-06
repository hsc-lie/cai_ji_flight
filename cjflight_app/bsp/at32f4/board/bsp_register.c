#include "bsp_register.h"

#include "at32f4xx.h"

#include "bsp_rcc.h"

#include "bsp_interrupt.h"

#include "gpio_dev.h"
#include "bsp_gpio.h"

#include "usart_dev.h"
#include "bsp_usart.h"
#include "ring_queue.h"

#include "i2c_dev.h"
#include "simulation_i2c.h"

#include "timer_dev.h"
#include "bsp_timer.h"


extern int main(void);


typedef struct
{
	GPIO_Type * Type;
	uint32_t Pin;
}GPIOPort_t;

static const GPIOPort_t GPIOMapping[GPIO_TYPE_MAX] = 
{
	{LED0_GPIO, LED0_GPIO_PIN},
	{LED1_GPIO, LED1_GPIO_PIN},
	{LED2_GPIO, LED2_GPIO_PIN},
	
	{SIMULATION_I2C_SCL_GPIO, SIMULATION_I2C_SCL_GPIO_PIN},
	{SIMULATION_I2C_SDA_GPIO, SIMULATION_I2C_SDA_GPIO_PIN},

};

static void GPIOWritePinOut(GPIO_TYPE_t type, uint8_t value)
{
	GPIOPort_t *port = &GPIOMapping[type];

	if(0u == value)
	{
		GPIO_ResetBits(port->Type, port->Pin);
	}
	else
	{
		GPIO_SetBits(port->Type, port->Pin);
	}
}

static uint8_t GPIOReadPinIn(GPIO_TYPE_t type)
{
	GPIOPort_t *port = &GPIOMapping[type];

	return GPIO_ReadInputDataBit(port->Type, port->Pin);
}

static USART_DEV_ERROR_t USART1SendData(uint8_t *data, uint32_t len)
{
	BSPUSARTSendData(USART1, data, len);

	return USART_DEV_ERROR_OK;
}

static USART_DEV_ERROR_t USART2ReadData(uint8_t *data, uint32_t readLen, uint32_t *outLen)
{
	*outLen = BSPUSART2ReadData(data, readLen);

	return USART_DEV_ERROR_OK;
}

static void USART2Init()
{
	BSPDMA1Cannel5Init();
	BSPUSART2Init();
}

#define SIMULATION_I2C_DELAY_COUNT            (20)

static void SimulationI2C1SCLSet(uint8_t value)
{
	GPIODevWritePinOut(GPIO_TYPE_SIMULATION_I2C_SCL, value);
}

static void SimulationI2C1SDASet(uint8_t value)
{
	GPIODevWritePinOut(GPIO_TYPE_SIMULATION_I2C_SDA, value);
}

static void SimulationI2C1SDADirSet(SIMULATION_I2C_SDA_DIR_t dir)
{
	if(SIMULATION_I2C_SDA_RX == dir)
	{
		GPIODevWritePinOut(GPIO_TYPE_SIMULATION_I2C_SDA, 1);
	}
}

static uint8_t SimulationI2C1SDARead()
{
	return GPIODevReadPinIn(GPIO_TYPE_SIMULATION_I2C_SDA);
}


static SimulationI2C_t SimulationI2C1 = 
{
	.DelayCount = SIMULATION_I2C_DELAY_COUNT,
	.SCLSet = SimulationI2C1SCLSet,
	.SDASet = SimulationI2C1SDASet,
	.SDADirSet = SimulationI2C1SDADirSet,
	.SDARead = SimulationI2C1SDARead,
};

static int I2C0SendData(uint8_t addr, uint8_t *reg, uint32_t regLen, uint8_t * data, uint32_t dataLen)
{
	return SimulationI2CWriteData(&SimulationI2C1, addr, reg, regLen, data, dataLen);
}

static int I2C0ReadData(uint8_t addr, uint8_t *reg, uint32_t regLen, uint8_t * data, uint32_t dataLen)
{
	return SimulationI2CReadData(&SimulationI2C1, addr, reg, regLen, data, dataLen);
}


static void Timer3PWMOut(uint8_t channel, uint32_t duty)
{
	switch (channel)
	{
		case 1:
			TMR_SetCompare1(TMR3, duty);
			break;
		case 2:
			TMR_SetCompare2(TMR3, duty);
			break;
		case 3:
			TMR_SetCompare3(TMR3, duty);
			break;
		case 4:
			TMR_SetCompare4(TMR3, duty);
			break;
		default:
			break;
	}
}

GPIODev_t GPIODev =
{
	.Init = BSPGPIOInitAll,
	.DeInit = NULL,
	.WritePinOut = GPIOWritePinOut,
	.ReadPinIn = GPIOReadPinIn,
};


USARTDev_t USART1Dev = 
{
	.Init = BSPUSART1Init,
	.DeInit = NULL,
	.SendData = USART1SendData,
	.ReadData = NULL,
};


USARTDev_t USART2Dev = 
{
	.Init = USART2Init,
	.DeInit = NULL,
	.SendData = NULL,
	.ReadData = USART2ReadData,
};


static I2CDev_t I2CDev0 =
{
	.Init = NULL,
    .DeInit = NULL,
	.SendData = I2C0SendData,
	.ReadData = I2C0ReadData,
};


static TimerDev_t Timer3Dev =
{
	.Init = BSPTimer3Init,
    .DeInit = NULL,
	.PWMOut = Timer3PWMOut,
};


void BSPMain(void)
{
	BSPInterruptInit();

	BSPRCCInitAll();

	GPIODevRegister(&GPIODev);

	USARTDevRegister(USART_PRINTF, &USART1Dev);
	USARTDevRegister(USART_REMOTE, &USART2Dev);

	I2CDevRegister(I2C_TYPE_SENSOR, &I2CDev0);

	TimerDevRegister(TIMER_MOTOR_PWM, &Timer3Dev);

	main();
}


