#include "stdint.h"

//Enums
typedef enum 
{
    NOT_STARTED = 0U,
    STARTED,
    PAUSED,
    ENDED
} game_state_t;

typedef enum 
{
    PLAYER1 = 0U,
    PLAYER2
} player_t;

//Structs
typedef struct 
{
    uint8_t minutes;
    uint8_t seconds;
} player_time_t;

typedef struct 
{
    volatile game_state_t state;
    volatile player_time_t startTime;
    volatile uint8_t bonusSeconds; //Bonus time before each move
    volatile player_time_t times[2];
    volatile player_t currentPlayer;
} game_t;

game_t game;

void setTimeForBothPlayers (game_t *game);
void gameInit (game_t *game);
void decrementTimeForActivePlayer (game_t *game);
void switchCurrentPlayer (game_t *game);
void switchGameState (game_t *game);

