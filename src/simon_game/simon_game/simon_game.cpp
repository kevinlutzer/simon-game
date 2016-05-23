/*
 * simon_game_2.c
 *
 * Created: 2015-02-15 7:33:35 PM
 * Author: Kevin
*/ 

/*
General Defs

Led1 = PD2
Led2 = PB7
MISO = PB4
Led3 = PC1

BUT1 = PD3
BUT2 = PD5
SCK = PB5
BUT3 = PC2

BUT1 =  PCINT19
BUT2 = PCINT21
BUT3 = PCINT10
SCK = PCINT5

Note: All buttons are active low, using the atmegas internal pullups. 
Note: Use a debounce time of 100 ms 
Note: for whatever reason the complier doesn't like comparing complex variable types directly.
*/

// includes 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <stdlib.h>

// global vars 
struct list_el{
	// true means a button was pressed false means otherwise
	bool BUT1,BUT2,SCK,BUT3;
};

int k = 100;// size of list
int j = 0; // the sequence index the player is up too 
int i = 0; // the incrementation variable through the memorized sequence
list_el list[100]; // list containing random button sequences to memorize. 
list_el roundList; // sequence of button presses for the current round the player is on 
int const delayTime = 1000;

// general flags 
bool commonFlag = false; // if one of the buttons is pressed during a round, this flag is changed to true.
bool stillCorrect = true; // if the player is still correct this flag remains true. 
bool checkingRound = false; 

// turn off all leds
void ClearLeds(void)
{
	PORTD &= ~(1 << PORTD2);
	PORTB &= ~(1 << PORTB7);
	PORTB &= ~(1 << PORTB4);
	PORTC &= ~(1 << PORTC1);
}
// initialize the gpio
void InitGpio(void)
{
	
	// LEDs 
	DDRD |= (1 << DDD2);  // Led 1  
	DDRB |= (1 << DDB7) | (1 << DDB4); // Led 2 and 4
	DDRC |= (1 << DDC1); // Led 3

	// PBs 
	DDRD &= (~(1 << DDD3) | ~(1 << DDD5));
	DDRC &= ~(1 << DDC2);
	DDRB &= ~(1 << DDB5);
	
	// Pullups
	PORTC |= (1 << PORTC2);
	PORTD |= (1 << PORTD3) | (1 << PORTD5);
	PORTB |= (1 << PORTB5);
		
}
// initialize the hardware interrupts
void InitInterrupts(void)
{
	// enable interrupt masks
	PCICR |= (1 << PCIE2);
	PCICR |= (1 << PCIE1);
	PCICR |= (1 << PCIE0);
	
	// enable pin change interrupts for certain GPIO
	PCMSK1 |= (1 << PCINT10);
	PCMSK2 |= (1 << PCINT19);
	PCMSK2 |= (1 << PCINT21);
	PCMSK0 |= (1 << PCINT5);
	
	// enable the global interrupts
	sei();
}
// blinks the leds at a frequency of 2 hz 
void BlinkLeds(void)
{
	PORTD |= (1 << PORTD2);
	PORTB |= (1 << PORTB7) | (1 << PORTB4);
	PORTC |= (1 << PORTC1);
	
	_delay_ms(250);
	ClearLeds();
	_delay_ms(250);
}
// lights the led correspond to the PB that is pressed
void TestButtons(void)
{
	if ( (PIND & (1 << PINC3) ) == 0)
	{
		PORTD |= (1 << PORTD2);
	} 
	else
	{
		PORTD &= ~(1 << PORTD2);
	}
	if ( (PIND & (1 << PIND5) ) == 0)
	{
		PORTB |= (1 << PORTB7);
	} 
	else
	{
		PORTB &= ~(1 << PORTB7);
	}
	if ( (PINB & (1 << PINB5) ) == 0)
	{
		PORTB |= (1 << PORTB4); 
	} 
	else
	{
		PORTB &= ~(1 << PORTB4);
	}	
	if ( (PINC & (1 << PINC2) ) == 0)
	{
		PORTC |= (1 << PORTC1);
	} 
	else
	{
		PORTC &= ~(1 << PORTC1);
	}
}
// fills the created list with random sequences
void FillList(void)
{
	int r_num;
	for(int ii = 0; ii < k; ii ++)
	{
		r_num = rand() % 256;
		if(r_num < 64)
		{
			list[ii].BUT1 = true;
			list[ii].BUT2 = false;
			list[ii].SCK = false;
			list[ii].BUT3 = false;
		}
		else if(r_num > 63 && r_num < 128)
		{
			list[ii].BUT1 = false;
			list[ii].BUT2 = true;
			list[ii].SCK = false;
			list[ii].BUT3 = false;
		}
		else if(r_num > 127 && r_num < 192)
		{
			list[ii].BUT1 = false;
			list[ii].BUT2 = false;
			list[ii].SCK = true;
			list[ii].BUT3 = false;
		}
		else if(r_num > 191)
		{
			list[ii].BUT1 = false;
			list[ii].BUT2 = false;
			list[ii].SCK = false;
			list[ii].BUT3 = true;
		}
	}
}
// clear flags 
void ClearFlags(void)
{
	commonFlag = false;
}
// Determine if a button has been pressed
bool CheckPress(void)
{
	if((PIND & (1 << PINC3) ) == 0 || 
	   (PIND & (1 << PIND5) ) == 0 ||
	   (PINB & (1 << PINB5) ) == 0 ||
	   (PINC & (1 << PINC2) ) == 0
	)
	{
		return true;
	}
	else 
	{
		return false;
	}
}
// initialize the adc module 
void InitAdc()
{
	ADCSRA |= _BV(ADEN);
}
// read data from the adc module 
uint16_t AdcRead(void) {
	ADMUX	&=	0xf0;
	ADMUX	|=	0;
 	ADCSRA |= _BV(ADSC);
 	while ( (ADCSRA & _BV(ADSC)) );
 	return ADC;
}
// display the list sequence using leds. 
void DisplayListSequence(void)
{
	list[4].BUT1 == true;
	for(int ii = j; ii >= 0 ; ii --)
	{
		if(commonFlag)
		{
			break;
		}
		else if(list[ii].BUT1)
		{
			PORTD |= (1 << PORTD2);
			PORTB &= ~(1 << PORTB7);
			PORTB &= ~(1 << PORTB4);
			PORTC &= ~(1 << PORTC1);
		}
		else if(list[ii].BUT2)
		{
			PORTD &= ~(1 << PORTD2);
			PORTB |= (1 << PORTB7);
			PORTB &= ~(1 << PORTB4);
			PORTC &= ~(1 << PORTC1);
		}
		else if(list[ii].SCK)
		{
			PORTD &= ~(1 << PORTD2);
			PORTB &= ~(1 << PORTB7);
			PORTB |= (1 << PORTB4);
			PORTC &= ~(1 << PORTC1);
		}
		else if(list[ii].BUT3)
		{
			PORTD &= ~(1 << PORTD2);
			PORTB &= ~(1 << PORTB7);
			PORTB &= ~(1 << PORTB4);
			PORTC |= (1 << PORTC1);
		}
		_delay_ms(250);
		ClearLeds();
		_delay_ms(250);
	}
}
// function that determines if the user entered in the correct sequence.
bool CheckList(void)
{
	if( 
		roundList.BUT1 == list[k].BUT1 &&
		roundList.BUT2 == list[k].BUT2 &&
		roundList.SCK == list[k].SCK &&
		roundList.BUT3 == list[k].BUT3
		)
	{
		return true;
	}
	else 
	{
		return false;
	}
}
int main()
{
	InitGpio();
	InitInterrupts();
	InitAdc();
	srand(AdcRead()); // seed random number generator;
	FillList(); // create list
	stillCorrect = true;
	j = 0; 
	k = 0;
	while(1)
	{
		while(stillCorrect)
		{
			
			k = j;
			DisplayListSequence();
			checkingRound = true;
			while(k >= 0 && stillCorrect)
			{
				_delay_ms(delayTime);
			}
			ClearFlags();	
			j ++;
		}
		BlinkLeds();
	}
}
// interrupt services
ISR (PCINT0_vect)
{
	if ( (PINB & (1 << PINB5)) == 0)
	{
		commonFlag = true; 
		PORTB |= (1 << PORTB4);			 
		k --; // to signify that we are checking to see if buttons have been pressed
		roundList.BUT1 = false;
		roundList.BUT2 = false; 
		roundList.SCK = true;
		roundList.BUT3 = false; 
		_delay_ms(250); 
		stillCorrect = CheckList();
		PORTB &= ~(1 << PORTB4); 
	}
}
ISR (PCINT1_vect)
{
	if ( (PINC & (1 << PINC2)) == 0)
	{
		commonFlag = true;
		
			 PORTC |= (1 << PORTC1);
			 k --; // to signify that we are checking to see if buttons have been pressed
			 roundList.BUT1 = false;
			 roundList.BUT2 = false;
			 roundList.SCK = false;
			 roundList.BUT3 = true;
			 _delay_ms(250);
			 stillCorrect = CheckList();
		 	 PORTC &= ~(1 << PORTC1);
	} 
}
ISR (PCINT2_vect)
{
	if ( (PIND & (1 << PIND5)) == 0) 
	{ 
		commonFlag = true;
		
			PORTB |= (1 << PORTB7); 
			k --; // to signify that we are checking to see if buttons have been pressed
			roundList.BUT1 = false;
			roundList.BUT2 = true;
			roundList.SCK = false;
			roundList.BUT3 = false;
			_delay_ms(250);
			stillCorrect = CheckList();
			PORTB &= ~(1 << PORTB7);
	} 
	
	if ( (PIND & (1 << PIND3)) == 0)
	{ 
		commonFlag = true;
		
			PORTD |= (1 << PORTD2);
			k --; // to signify that we are checking to see if buttons have been pressed
			roundList.BUT1 = true;
			roundList.BUT2 = false;
			roundList.SCK = false;
			roundList.BUT3 = false;
			_delay_ms(250);
			stillCorrect = CheckList();
			PORTD &= ~(1 << PORTD2);
	} 
}
