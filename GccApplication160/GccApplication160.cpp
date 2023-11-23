#define F_CPU 8000000UL
#define BAUD 9600
#define UBRR_VAL F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

int data[5] = {0,0,0,0,0};
int SateDataArr[1] = {1};
int InputData = 0;
char StateData;
int Exp = 0;

int i = 0;
int CounterChange = 0;
bool CounterPSF1 = false;
int OutData = 0;
int TriggerStart = 1;
int StartDataState = 0;
int Start = 1;
bool StatePSF1 = false;

int StateOutPut = 0;

bool State = false;
bool NewState = true;
bool flag = false;
bool Enabel = false;
int n = 0;
char U = 'A';

bool Test = false;
ISR(USART_RXC_vect)
{
	StateData = UDR;
	switch(StateData)
	{
		case '0': //0_SW_ON = 0;
		SateDataArr[0] = 0;
		CounterChange = 1;		
		break;
		case '1': //0_SW_ON = 1;
		SateDataArr[0] = 1;
		CounterChange = 1;	
		break;
		case 'a':
		StatePSF1 = true;
		CounterPSF1 = true;
		break;
		case 'b':
		StatePSF1 = false;
		CounterPSF1 = true;
		break;		
	}
}
void usart_init(unsigned int speed)
{
	// устанавливаем скорость Baud Rate: 9600
	UBRRH=(unsigned char)(speed>>8);
	UBRRL=(unsigned char) speed;
	UCSRA=0x00;
	UCSRB|=(1<<RXEN)|(1<<TXEN);// –азрешение работы приемника и передатчика
	UCSRB|=(1<<RXCIE);// –азрешение прерываний по приему
	// ”становка формата посылки: 8 бит данных, 1 стоп-бит
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

int main(void)
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC = 0x00;
	PORTB = (1<<PB0);
	PORTA = (1<<PA0);	
	
	usart_init(UBRR_VAL);
	sei();
	
	while(1)
	{
		if (bit_is_set(PINC, PINC1))
		{
			NewState = true;
			Test = true;
		}
		if (bit_is_clear(PINC, PINC1))
		{
			NewState = false;
			Test = false;
		}
		if (NewState != State)
		{	
			if (Test)
			{
//				UDR = 'a';
			}
			else
			{
//				UDR = 'b';	
			}					
			flag = true;			
			State = NewState;
		}
		
/*		if (flag)
		{
			Enabel = true;
			flag = false;
		}
		if (Enabel)
		{
		if (bit_is_set(PINC, PINC1))
		{
			UDR = 'E';
		}
		if (bit_is_clear(PINC, PINC1))
		{
			UDR = 'N';
		}		
			Enabel = false;
		}*/
		_delay_ms(1);
		if (TriggerStart == 1)
		{
			if (SateDataArr[0] == 1)
			{
				UDR = '1';
				_delay_ms(1);
				UDR = 'p';
			}
			TriggerStart = 0;
		}
		if (CounterChange == 1)
		{
			if (SateDataArr[0] == 0)
			{
				PORTB &= ~(1<<PB0);
				PORTA &= ~(1<<PA0);
				PORTA |= (1<<PA1);
				PORTB |= (1<<PB2);				
				_delay_ms(1);
				if (bit_is_clear(PINC, PINC0))
				{
					UDR = '0';
				}							
				if (bit_is_set(PINC, PINC1))
				{
					UDR = 'r';
				}
				_delay_ms(1);
/*			if (!(PINC1&(1<<PINC1)))
			{
				StatePSF1 = 0;
				PORTB &= ~(1<<PB2); 
				UDR = 'a';				
			}*/				
			}
			
			if (SateDataArr[0] == 1)
			{
				PORTB |= (1<<PB0);
				PORTA |= (1<<PA0);
				PORTA &= ~(1<<PA1);
				PORTB &= ~(1<<PB2);
				_delay_ms(1);
				if (bit_is_set(PINC, PINC0))
				{
					UDR = '1';
				}		
				if (bit_is_clear(PINC, PINC1))
				{
					UDR = 'p';
				}
				_delay_ms(1);
			}
/*			if (PINC1 == 0)
			{
				StatePSF1 = 1;
				PORTB |= (1<<PB2); 
				UDR = 'b';				
			}				
			}
			_delay_ms(1);
			if (StatePSF1 == 0)
			{
				UDR = 'a';
				PORTB &= ~(1<<PB7);				

			}
			if (StatePSF1 == 1)
			{
				UDR = 'b';
				PORTB |= (1<<PB7); 
			}*/
			CounterChange = 0;
		}
		if (CounterPSF1)
		{

			if (StatePSF1)
			{
				PORTB &= ~(1<<PB1);
				_delay_ms(1);
				UDR = 'a';
			}
			else
			{
				PORTB |= (1<<PB1);
				_delay_ms(1);
				UDR = 'b';				
			}
			CounterPSF1 = false;
		}
				
	}
}