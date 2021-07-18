/*
 * Interfacing_1_Project.c
 *
 *  Created on: Oct 16, 2020
 *      Author: ABDELRAHMAN KASSEM
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define ONE_SECOND 15625      /*Number of ticks to reach 1 sec */
unsigned char clock[6] = {0}; /* clock[0] and clock[1] represent the seconds
								 clock[2] and clock[3] represent the minutes
								 clock[4] and clock[5] represent the hours*/
void INT0_1_2_config(void)
{
	DDRD &= ~(1<<PD2) & ~(1<<PD3);
	DDRB &= ~(1<<PB2);
	PORTD |= (1<<PD2);
	PORTB |= (1<<PB2);
	MCUCSR &= ~(1<<ISC2);
	GICR |= (1<<INT0) | (1<<INT1) | (1<<INT2);
	MCUCR = (1<<ISC01) | (1<<ISC10) | (1<<ISC11);
}
void timer1_config(void)
{
	TIMSK |= (1<<OCIE1A);
	TCNT1 = 0;
	OCR1A = ONE_SECOND;
	TCCR1A = (1<<FOC1A) ;
	TCCR1B = (1<<CS10) | (1<<CS11) | (1<<WGM12);
}
ISR (INT0_vect)
{
	unsigned char i;
	for(i=0 ; i<6 ; i++)
	{
		clock[i] = 0;      /*reset the timer values*/
	}
}
ISR (INT1_vect)
{
	TIMSK &= ~(1<<OCIE1A);  /*pause the timer*/
}
ISR (INT2_vect)
{
	TIMSK |= (1<<OCIE1A);   /*resume the timer*/
}

ISR (TIMER1_COMPA_vect)    /*incrementing the clock by one second each interrupt*/
{
	clock[0]++;
	if(clock[0] == 10)
	{
		clock[0] = 0;
		clock[1]++;
	}
	if(clock[1] == 6)
	{
		clock[1]= 0;
		clock[2]++;
	}
	if(clock[2] == 10)
	{
		clock[2]= 0;
		clock[3]++;
	}
	if(clock[3] == 6)
	{
		clock[3]= 0;
		clock[4]++;
	}
	if(clock[4] == 10)
	{
		clock[4]= 0;
		clock[5]++;
	}
	if(clock[5] == 10)
	{
		clock[5]= 0;
	}
}

int main(void)
{
	SREG |= (1<<7);
	INT0_1_2_config();
	timer1_config();
	unsigned char i;
	DDRA |= 0X0F;       /*using the first 4 pins in porta to display numbers on the 7-segment*/
	PORTA &= 0XF0;
	DDRC |= 0X3F;  					/*using the first six pins in portc as output pins
									to enable and disable the 7-segment sequentially*/
	PORTC &= ~(0X3F);
	while(1)
	{
		for(i=0 ; i<6 ; i++ )
		{
			PORTC |= (1<<i);
			PORTA = (PORTA & 0xF0) | clock[i];
			_delay_ms(2);
			PORTC &= ~(1<<i);
		}
	}
}
