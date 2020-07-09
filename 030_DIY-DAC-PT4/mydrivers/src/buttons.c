#include "buttons.h"

Button_t Buttons[BUTTONS_NO];

/**
* @brief  Init all used buttons
* @info		Use Button_t for each button
**/
void buttonInit(void)
{
	Buttons[BUP].pin = BUTTON_UP_Pin;
	Buttons[BUP].port = BUTTON_UP_Port;

	Buttons[BOK].pin = BUTTON_OK_Pin;
	Buttons[BOK].port = BUTTON_OK_Port;

	Buttons[BDOWN].pin = BUTTON_DOWN_Pin;
	Buttons[BDOWN].port = BUTTON_DOWN_Port;
}

/**
* @brief  Read button state with debouncing
* @param	i button position in list BtnPos for array Buttons
**/
uint8_t buttonRead(BtnPos i)
{
	// Button read routine with debounce of 10 readings
	while(1)
	{
		// Increment debounce
		if( Buttons[i].debounce != 0 )
		{
			Buttons[i].debounce++;
		}
		// Read button and check for new state if not already debouncing
		Buttons[i].state = LL_GPIO_IsInputPinSet(Buttons[i].port, Buttons[i].pin);
		// Exit if zero-state
		if( (Buttons[i].state == 0) && (Buttons[i].debounce == 0) )
		{
			return 0;
		}
		// Enter debouncing
		if ( (Buttons[i].state == 1) && (Buttons[i].debounce == 0) )
		{
			Buttons[i].debounce = 1;
		}
		// End of debounce
		if ( Buttons[i].debounce >= 10 )
		{
			Buttons[i].debounce = 0;
			// If still state 1
			if( Buttons[i].state == 1 )
			{
				Buttons[i].status = CLOSED;
				return 1;
			}
			// State 0 after 10 readings means a glitch
			else
			{
				Buttons[i].status = OPEN;
				return 0;
			}
		}
	}
}
