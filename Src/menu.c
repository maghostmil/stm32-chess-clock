#include "menu.h"

#include "stm32f1xx_hal.h"
#include "tim.h"
#include "stm32_tm1637.h"
#include "game.h"
#include "mode.h"

const player_time_t gameTimes[] = {
    {2,0}, //0
    {3,0}, //1
    {4,0}, //2
    {5,0}, //3
    {10,0}, //4
    {15,0}, //5
    {20,0}, //6
    {30,0}, //7
    {60,0}, //8
    {90,0} //9
};

void menuInit (menu_t *menu) 
{
    mode = MENU;

    //1. Set defulat values
    menu->currentOption = 0;
    menu->bonusSeconds = 0;
    menu->selectedTime = 5;
    menu->brightness = 8;

    tm1637DisplayText(display[0], menu->currentOption);
    tm1637DisplayClear(display[1]);
}

void nextMenuOption (menu_t *menu) 
{
    //1. Set next option
    menu->currentOption++;
    if (menu->currentOption > MENU_OPTIONS - 1) menu->currentOption = 0;

    switch (menu->currentOption)
    {
    case START_GAME:
        tm1637DisplayText(display[0], START_GAME);
        tm1637DisplayText(display[1], 4);
        break;

    case GAME_TIME:
        tm1637DisplayText(display[0], GAME_TIME);
        tm1637DisplayDecimal(display[1], gameTimes[menu->selectedTime].minutes*100 + gameTimes[menu->selectedTime].seconds, 1);
        break;

    case BONUS_TIME:
        tm1637DisplayText(display[0], BONUS_TIME);
        tm1637DisplayDecimal(display[1], menu->bonusSeconds, 0);
        break;

    case BRIGHTNESS:
        tm1637DisplayText(display[0], BRIGHTNESS);
        tm1637DisplayDecimal(display[1], menu->brightness, 0);
        break;

    default:
        break;
    }
}

void setMenuOption (menu_t *menu) 
{
    switch (menu->currentOption)
    {
    case START_GAME:
        game.bonusSeconds = menu->bonusSeconds;

        game.startTime.minutes = gameTimes[menu->selectedTime].minutes;
        game.startTime.seconds = gameTimes[menu->selectedTime].seconds;

        gameInit(&game);
        break;

    case GAME_TIME:
        menu->selectedTime++;
        if(menu->selectedTime >= GAME_TIMES) menu->selectedTime = 0;
        tm1637DisplayDecimal(display[1], gameTimes[menu->selectedTime].minutes*100 + gameTimes[menu->selectedTime].seconds, 1);
        break;

    case BONUS_TIME:
        menu->bonusSeconds++;
        if (menu->bonusSeconds > MAX_BONUS_TIME) menu->bonusSeconds = 0;
        tm1637DisplayDecimal(display[1], menu->bonusSeconds, 0);
        break;

    case BRIGHTNESS:
        menu->brightness++;
        if (menu->brightness > 8) menu->brightness = 1;
        tm1637DisplayDecimal(display[1], menu->brightness, 0);
        
        tm1637SetBrightness(display[0], menu->brightness);
        tm1637SetBrightness(display[1], menu->brightness);
        break;

    default:
        break;
    }
}