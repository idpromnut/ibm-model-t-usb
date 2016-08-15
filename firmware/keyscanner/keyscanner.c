
#include "keyscanner.h"
#include "stm32f1xx_hal.h"
#include "ibm-model-m-101.h"

// private functions
uint8_t KS_BitPositionToInteger(uint16_t iValue);
void KS_GpioPinInit(GPIO_TypeDef *iPort, uint32_t iPin, uint32_t iMode, uint32_t iPull, uint32_t iSpeed);

void KS_Init()
{
	// remap JTAG to GPIO, leaving SWD enabled
	__HAL_AFIO_REMAP_SWJ_NOJTAG();

	// init input scan lines
	KS_GpioPinInit(GPIOB, GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  1
	KS_GpioPinInit(GPIOB, GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  2
	KS_GpioPinInit(GPIOB, GPIO_PIN_2, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  3
	KS_GpioPinInit(GPIOB, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  4
	KS_GpioPinInit(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  5
	KS_GpioPinInit(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  6
	KS_GpioPinInit(GPIOB, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  7
	KS_GpioPinInit(GPIOB, GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  8
	KS_GpioPinInit(GPIOB, GPIO_PIN_8, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  //  9
	KS_GpioPinInit(GPIOB, GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW);  // 10
	KS_GpioPinInit(GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 11
	KS_GpioPinInit(GPIOB, GPIO_PIN_11, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 12
	KS_GpioPinInit(GPIOB, GPIO_PIN_12, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 13
	KS_GpioPinInit(GPIOB, GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 14
	KS_GpioPinInit(GPIOB, GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 15
	KS_GpioPinInit(GPIOB, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW); // 16

	// init output scan lines
	KS_GpioPinInit(GPIOA, GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
	KS_GpioPinInit(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
}


uint16_t KS_ScanForKeyPress(uint8_t iActiveLine)
{
	uint32_t outputRegister = (GPIOA->ODR & 0xFFFFFF00) | (uint32_t)iActiveLine;
	GPIOA->ODR = outputRegister;
	uint16_t scannedInput = (uint16_t)GPIOB->IDR;
	return scannedInput;
}

void KS_FormatScanLines(uint8_t iActiveLine, uint16_t iReadLines)
{
}

uint8_t KS_PrintScanLines()
{
	uint16_t scannedInput;
	uint8_t keyScans = 0;

	for(uint8_t i = 0; i < 8; ++i)
	{
		scannedInput = KS_ScanForKeyPress(1 << (7 - i));
		if (scannedInput > 0)
		{
			// detected a key press
			printf("Key press detected at %d/0x%04X\n", (i+1), scannedInput);
			++keyScans;
		}
	}

	return keyScans;
}

uint8_t KS_ReadScanCode()
{
	uint16_t scannedInput;

	for(uint8_t i = 0; i < 8; ++i)
	{
		scannedInput = KS_ScanForKeyPress(1 << (7 - i));
		if (scannedInput > 0)
		{
			// detected a key press
			//decode
			uint8_t readLine = KS_BitPositionToInteger(scannedInput);
			if (KEY_MAP[i][readLine] > 0) {
				return KEY_MAP[i][readLine];
			}
			else {
				printf("No key-code found for scanned key: %i/%i\n", i, readLine);
			}
		}
	}

	return 0;
}

uint8_t KS_BitPositionToInteger(uint16_t value)
{
	for(uint8_t i = 0; i < 16; ++i)
	{
		if ((value & 0x0001) == 1) return (0x0F - i);
		value = value >> 1;
	}
	return -1;
}


void KS_GpioPinInit(GPIO_TypeDef *iPort, uint32_t iPin, uint32_t iMode, uint32_t iPull, uint32_t iSpeed)
{
	// Initialize the port and the pin
	GPIO_InitTypeDef   GPIO_InitStructure;
	GPIO_InitStructure.Pin = iPin;
	GPIO_InitStructure.Mode = iMode;
	GPIO_InitStructure.Pull = iPull;
	GPIO_InitStructure.Speed = iSpeed;
	HAL_GPIO_Init( iPort, &GPIO_InitStructure );
}
