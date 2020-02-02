#include "stm32f1xx_hal.h"
#include "stm32_tm1637.h"


// Configuration.

#define DIS1_CLK_PORT               GPIOB
#define DIS1_DIO_PORT               GPIOB
#define DIS1_CLK_PIN                GPIO_PIN_10
#define DIS1_DIO_PIN                GPIO_PIN_11

#define DIS2_CLK_PORT               GPIOB
#define DIS2_DIO_PORT               GPIOB
#define DIS2_CLK_PIN                GPIO_PIN_8
#define DIS2_DIO_PIN                GPIO_PIN_9

#define CLK_PORT_CLK_ENABLE         __HAL_RCC_GPIOB_CLK_ENABLE
#define DIO_PORT_CLK_ENABLE         __HAL_RCC_GPIOB_CLK_ENABLE

void _tm1637Start(display_t display);
void _tm1637Stop(display_t display);
void _tm1637ReadResult(display_t display);
void _tm1637WriteByte(display_t display, unsigned char b);
void _tm1637DelayUsec(unsigned int i);
void _tm1637ClkHigh(display_t display);
void _tm1637ClkLow(display_t display);
void _tm1637DioHigh(display_t display);
void _tm1637DioLow(display_t display);

display_t display[2] = { 
    {DIS1_DIO_PORT, DIS1_DIO_PIN, DIS1_CLK_PORT, DIS1_CLK_PIN},
    {DIS2_DIO_PORT, DIS2_DIO_PIN, DIS2_CLK_PORT, DIS2_CLK_PIN}
};

const char wordSegmentMap[5][4] = {
    {0x6d, 0x78, 0x77, 0x50}, //star
    {0x78, 0x10, 0x54, 0x44}, //tim
    {0x7c, 0x5c, 0x54, 0x1c}, //bonu
    {0x7c, 0x50, 0x10, 0x6f},  //brig
    {0x00, 0x00, 0x00, 0x00}  //clear
};

const char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};


void tm1637Init(display_t display)
{
    CLK_PORT_CLK_ENABLE();
    DIO_PORT_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pull = GPIO_PULLUP;
    g.Mode = GPIO_MODE_OUTPUT_OD; // OD = open drain
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Pin = display.clkPin;
    HAL_GPIO_Init(display.clkPort, &g);
    g.Pin = display.dioPin;
    HAL_GPIO_Init(display.dioPort, &g);
    tm1637SetBrightness(display, 8);
}

void tm1637DisplayDecimal(display_t display, int v, int displaySeparator)
{
    unsigned char digitArr[4];
    for (int i = 0; i < 4; ++i) {
        digitArr[i] = segmentMap[v % 10];
        if (i == 2 && displaySeparator) {
            digitArr[i] |= 1 << 7;
        }
        v /= 10;
    }

    _tm1637Start(display);
    _tm1637WriteByte(display, 0x40);
    _tm1637ReadResult(display);
    _tm1637Stop(display);

    _tm1637Start(display);
    _tm1637WriteByte(display, 0xc0);
    _tm1637ReadResult(display);

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(display, digitArr[3 - i]);
        _tm1637ReadResult(display);
    }

    _tm1637Stop(display);
}

void tm1637DisplayText(display_t display, uint8_t type)
{
    unsigned char digitArr[4];
    for (int i = 0; i < 4; ++i) {
        digitArr[i] = wordSegmentMap[type][i];
    }

    _tm1637Start(display);
    _tm1637WriteByte(display, 0x40);
    _tm1637ReadResult(display);
    _tm1637Stop(display);

    _tm1637Start(display);
    _tm1637WriteByte(display, 0xc0);
    _tm1637ReadResult(display);

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(display, digitArr[i]);
        _tm1637ReadResult(display);
    }

    _tm1637Stop(display);
}

void tm1637DisplayClear(display_t display)
{
    unsigned char digitArr[4];
    for (int i = 0; i < 4; ++i) {
        digitArr[i] = 0;
    }

    _tm1637Start(display);
    _tm1637WriteByte(display, 0x40);
    _tm1637ReadResult(display);
    _tm1637Stop(display);

    _tm1637Start(display);
    _tm1637WriteByte(display, 0xc0);
    _tm1637ReadResult(display);

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(display, digitArr[i]);
        _tm1637ReadResult(display);
    }

    _tm1637Stop(display);
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(display_t display, char brightness)
{
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    _tm1637Start(display);
    _tm1637WriteByte(display, 0x87 + brightness);
    _tm1637ReadResult(display);
    _tm1637Stop(display);
}

void _tm1637Start(display_t display)
{
    _tm1637ClkHigh(display);
    _tm1637DioHigh(display);
    _tm1637DelayUsec(2);
    _tm1637DioLow(display);
}

void _tm1637Stop(display_t display)
{
    _tm1637ClkLow(display);
    _tm1637DelayUsec(2);
    _tm1637DioLow(display);
    _tm1637DelayUsec(2);
    _tm1637ClkHigh(display);
    _tm1637DelayUsec(2);
    _tm1637DioHigh(display);
}

void _tm1637ReadResult(display_t display)
{
    _tm1637ClkLow(display);
    _tm1637DelayUsec(5);
    // while (dio); // We're cheating here and not actually reading back the response.
    _tm1637ClkHigh(display);
    _tm1637DelayUsec(2);
    _tm1637ClkLow(display);
}

void _tm1637WriteByte(display_t display, unsigned char b)
{
    for (int i = 0; i < 8; ++i) {
        _tm1637ClkLow(display);
        if (b & 0x01) {
            _tm1637DioHigh(display);
        }
        else {
            _tm1637DioLow(display);
        }
        _tm1637DelayUsec(3);
        b >>= 1;
        _tm1637ClkHigh(display);
        _tm1637DelayUsec(3);
    }
}

void _tm1637DelayUsec(unsigned int i)
{
    for (; i>0; i--) {
        for (int j = 0; j < 10; ++j) {
            __asm__ __volatile__("nop\n\t":::"memory");
        }
    }
}

void _tm1637ClkHigh(display_t display)
{
    HAL_GPIO_WritePin(display.clkPort, display.clkPin, GPIO_PIN_SET);
}

void _tm1637ClkLow(display_t display)
{
    HAL_GPIO_WritePin(display.clkPort, display.clkPin, GPIO_PIN_RESET);
}

void _tm1637DioHigh(display_t display)
{
    HAL_GPIO_WritePin(display.dioPort, display.dioPin, GPIO_PIN_SET);
}

void _tm1637DioLow(display_t display)
{
    HAL_GPIO_WritePin(display.dioPort, display.dioPin, GPIO_PIN_RESET);
}
