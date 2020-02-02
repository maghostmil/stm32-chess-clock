#include "game.h"

#include "stm32f1xx_hal.h"
#include "tim.h"
#include "stm32_tm1637.h"
#include "mode.h"

//Procedures

void setTimeForBothPlayers (game_t *game) 
{
    game->times[PLAYER1].minutes = game->startTime.minutes;
    game->times[PLAYER1].seconds = game->startTime.seconds;

    game->times[PLAYER2].minutes = game->startTime.minutes;
    game->times[PLAYER2].seconds = game->startTime.seconds;
}

void gameInit (game_t *game) 
{
    //1. Set game mode
    mode = GAME;
    
    //2. Set game state to not started
    game->state = NOT_STARTED;

    //3. Set time for both players from game start time
    setTimeForBothPlayers(game);

    //4. Set current player
    game->currentPlayer = !HAL_GPIO_ReadPin(SIDE_KEY_GPIO_Port, SIDE_KEY_Pin);

    //5.Display times
    tm1637DisplayDecimal(display[0], game->times[PLAYER1].minutes*100 + game->times[PLAYER1].seconds, 1);
    tm1637DisplayDecimal(display[1], game->times[PLAYER2].minutes*100 + game->times[PLAYER2].seconds, 1);
}

void decrementTimeForActivePlayer (game_t *game)
{
    player_t currentPlayer = game->currentPlayer;

    if(game->state == STARTED) 
    {
        if (game->times[currentPlayer].seconds == 0) 
        {
            if (game->times[currentPlayer].minutes == 0) {
                //Times Gone! Game ended
                HAL_TIM_Base_Stop_IT(&htim1);
                game->state = ENDED;
            } 
            else 
            {
                game->times[currentPlayer].seconds = 59;
                game->times[currentPlayer].minutes --;
            }
        } 
        else 
        {
            game->times[currentPlayer].seconds --;
        }

        tm1637DisplayDecimal(display[currentPlayer], game->times[currentPlayer].minutes*100 + game->times[currentPlayer].seconds, (game->times[currentPlayer].seconds % 2 == 0) ? 1 : 0);
    }
}

void switchCurrentPlayer (game_t *game) 
{
    player_t currentPlayer = game->currentPlayer;

    switch (game->state)
    {
        case NOT_STARTED:

            game->state = STARTED;
            HAL_TIM_Base_Start_IT(&htim1);

        case STARTED:
            
            //1. Add bonus time to player
            game->times[currentPlayer].seconds = game->times[currentPlayer].seconds + game->bonusSeconds;

            if (game->times[currentPlayer].seconds >= 60) {
                game->times[currentPlayer].seconds = game->times[currentPlayer].seconds - 60;
                game->times[currentPlayer].minutes ++;
            }

            if (game->times[currentPlayer].minutes >= game->startTime.minutes) {
                if (game->times[currentPlayer].seconds > game->startTime.seconds){
                    game->times[currentPlayer].seconds = game->startTime.seconds;
                }
            }

            //2. Set colon ON for unactive player
            tm1637DisplayDecimal(display[currentPlayer], game->times[currentPlayer].minutes*100 + game->times[currentPlayer].seconds, 1);

            //3. Switch current player
            game->currentPlayer = !game->currentPlayer;

            //4. Restart counter
            __HAL_TIM_SET_COUNTER(&htim1, 0);
            break;

        default:
            break;
    }
}

void switchGameState (game_t *game) 
{
    switch (game->state)
    {
    case STARTED:
        game->state = PAUSED;
        HAL_TIM_Base_Stop_IT(&htim1);
        break;

    case PAUSED:
        game->state = STARTED;
        HAL_TIM_Base_Start_IT(&htim1);
        break;
    
    default:
        break;
    }
}

