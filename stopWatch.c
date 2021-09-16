#include<avr/io.h>
#include<avr/interrupt.h>
#include"util/delay.h"

#define DELAY 5

unsigned char sec0=0; //first digit  in seconds
unsigned char sec1=0;  //second digit in seconds
unsigned char min0=0;
unsigned char min1=0;
unsigned char hour0=0;
unsigned char hour1=0;


void timer1_init()
{
	TCNT1=0;
	OCR1A=997;   // counting till 997 equivalent to 0ne second
	TIMSK|=(1<<OCIE1A);//enable timer 1 interrupt
	/*
 compare mode channel A
 prescaler 1024
	 */
	TCCR1A=(1<<FOC1A);
	TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);
}

ISR(TIMER1_COMPA_vect)//incrementing time
{
	sec0++;

	if(sec0>9)
	{
		sec0=0;
		sec1++;
	}
	if(sec1>=6)
	{
		min0++;
		sec0=0;
		sec1=0;
	}
	if(min0>9)
	{
		min1++;
		min0=0;
		sec0=0;
		sec1=0;
	}
	if(min1>=6)
	{
		hour0++;
		min1=0;
		min0=0;
		sec0=0;
		sec1=0;
	}
	if(hour0>9)
	{
		hour1++;
		hour0=0;
		min1=0;
		min0=0;
		sec0=0;
		sec1=0;
	}
	if(hour1 >12)//counting from 0 again
	{
		TCNT1=0;
		sec0=0;
		sec1=0;
		min0=0;
		min1=0;
		hour0=0;
		hour1=0;
	}
}



void Int0_rest()//interrupt 0 on pin D2 to restart the counter
{
	DDRD  &= (~(1<<PD2));
	PORTD|=(1<<PD2);//internal pull up resistor
	GICR  |= (1<<INT0);
	MCUCR |= (1<<ISC01);//falling edge
	MCUCR&=~(1<<ISC00);
}


ISR(INT0_vect)
{
	TCNT1=0;
	sec0=0;
	sec1=0;
	min0=0;
	min1=0;
	hour0=0;
	hour1=0;
}




ISR(INT1_vect)
{
	if(PIND&(1<<PD3))
	{
		TIFR|=(1<<OCF1A);
		TCCR1A=0;
		TCCR1B=0;
	}
}


void Int1_pause()//interrupt 1 on pin D3 to stop the counter
{
	DDRD  &= (~(1<<PD3));// external pull down resistor
	GICR  |= (1<<INT1);//enable external interrupt for interrupt 1
	MCUCR |= (1<<ISC11)|(1<<ISC10);//rising edge
}






void Int2_resume()//Resume the counter
{
	DDRB  &= (~(1<<PB2));
	PORTB|=(1<<PB2);//internal pull up resistor
	GICR  |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2);
}
ISR(INT2_vect)
{
	if(!(PINB&(1<<PB2)))
	{
		TCCR1A=(1<<FOC1A);
		TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);
	}
}



int main()
{
	DDRC|=0x0f;//define the first 4 pins in portC as output pins
	PORTC&=0xf0;

	DDRA|=0x3f; //define the first 6 pins in portA as input pins
	PORTA|=0x00;

	timer1_init();
	Int0_rest();
	Int1_pause();
	Int2_resume();

	SREG|=(1<<7);
	while(1)
	{
		/*
		 using multiplexed technique to use only the same PORTC 4  pins
		 by enabling one segment for very small time and loop for the other
		 */
		PORTA=(1<<0);
		PORTC=sec0;
		_delay_ms(DELAY);
		PORTA&=~(1<<0);

		PORTA|=(1<<1);
		PORTC=sec1;
		_delay_ms(DELAY);
		PORTA&=~(1<<1);

		PORTA|=(1<<2);
		PORTC=min0;
		_delay_ms(DELAY);
		PORTA&=~(1<<2);

		PORTA|=(1<<3);
		PORTC=min1;
		_delay_ms(DELAY);
		PORTA&=~(1<<3);

		PORTA|=(1<<4);
		PORTC=hour0;
		_delay_ms(DELAY);
		PORTA&=~(1<<4);

		PORTA|=(1<<5);
		PORTC=hour1;
		_delay_ms(DELAY);
		PORTA&=~(1<<5);

	}
}
