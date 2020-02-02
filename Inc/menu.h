#include "stdint.h"

#define MENU_OPTIONS    4
#define GAME_TIMES      10
#define MAX_BONUS_TIME  15

typedef enum 
{
    START_GAME = 0U,
    GAME_TIME,
    BONUS_TIME,
    BRIGHTNESS
} menu_option_t;

typedef struct 
{
    volatile menu_option_t currentOption;
    volatile uint8_t bonusSeconds; //Bonus time before each move
    volatile uint8_t selectedTime;
    volatile uint8_t brightness;
} menu_t;

menu_t menu;

void menuInit (menu_t *menu);
void nextMenuOption (menu_t *menu);
void setMenuOption (menu_t *menu);
