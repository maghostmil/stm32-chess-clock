#pragma once

typedef struct {
    GPIO_TypeDef* dioPort;
    uint16_t dioPin;
    GPIO_TypeDef* clkPort;
    uint16_t clkPin;
} display_t;

display_t display[2];

void tm1637Init(display_t display);
void tm1637DisplayDecimal(display_t display, int v, int displaySeparator);
void tm1637DisplayText(display_t display, unsigned char text);
void tm1637DisplayClear(display_t display);
void tm1637SetBrightness(display_t display, char brightness);