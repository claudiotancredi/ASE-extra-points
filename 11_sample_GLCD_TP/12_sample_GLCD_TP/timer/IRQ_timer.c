/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../adc/adc.h"

extern uint8_t *text;
extern int game_status;

uint16_t SinTable[45] =                                       /*                      */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


void TIMER0_IRQHandler (void)
{
	disable_timer(0);
	reset_timer(0);
	ADC_start_conversion();
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	static uint8_t *text3 = (uint8_t *)"3";
	static uint8_t *text2 = (uint8_t *)"2";
	static uint8_t *text1 = (uint8_t *)"1";
	static uint8_t *textempty = (uint8_t *)" ";
	static int counter = 3;
	disable_timer(1);
	reset_timer(1);
	if (counter>=0){
		switch(counter){
			case 3: GUI_Text(116, 156, text3, White, Black);
				break;
			case 2: GUI_Text(116, 156, text2, White, Black);
				break;
			case 1:GUI_Text(116, 156, text1, White, Black);
				break;
			case 0:
				GUI_Text(116, 156, textempty, White, Black);
			game_status=1;
				break;
			default:
				break;
		}
		if(counter>0){
			counter--;
		init_timer(1, 0x17D7840); /*1 s * 25 MHz = 25*10^6=0x17D7840*/
	enable_timer(1);
		}
		else{
			counter=3;
			init_timer(0,0x618A); //1 ms
			//init_timer(0, 0x98968 ); 						/* 25ms * 25MHz = 6.25*10^5 = 0x98968 */
	enable_timer(0);
		}
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
	static int ticks=0;
	static int end_sound=0;
	disable_timer(0);
	reset_timer(0);
	/* DAC management */	
	LPC_DAC->DACR = SinTable[ticks]<<6;
	ticks++;
	end_sound++;
	if(ticks==45) {
		ticks=0;
	}
	if (end_sound==47){
		disable_timer(2);
		ticks=0;
		end_sound=0;
		init_timer(0,0x618A); //1 ms
		enable_timer(0);
	}
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
