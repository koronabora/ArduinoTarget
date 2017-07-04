#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <math.h>

#define F_CPU 16000000UL
#define BAUDRATE 250000
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

// Параметры мишени
/*
Расположение приемников в мишени:

|------|    |------|
|A    B|	|1    2|
|D    C|	|4    3|
|------|	|------|
   ||		   ||
*/
#define ID 1 // Номер мишени. 1 для АК, 3 для ПМ, 5 для Ярыгина
#define VA 0.9 // Коэффицент первого приемника
#define VB 0.9 // Коэффицент второго приемника
#define VC 0.9// Коэффицент третьего приемника
#define VD 0.9 // Коэффицент четвертого приемника

void adc_init(void);            		//Function to initialize/configure the ADC
uint16_t read_adc(uint8_t channel);   	//Function to read an arbitrary analogic channel/pin
void USART_init(void);            		//Function to initialize and configure the USART/serial
void USART_send( unsigned char data);   //Function that sends a char over the serial port
void USART_putstring(char* StringPtr);  //Function that sends a string over the serial port
uint16_t lowpass(uint16_t prev, uint16_t cur, float a);
void ULongToStr(uint16_t Value);


int main(void){
	uint16_t a0 = 0;
	uint16_t a1 = 0;
	uint16_t a2 = 0;
	uint16_t a3 = 0;

	uint16_t d01 = 0;
	uint16_t d11 = 0;
	uint16_t d21 = 0;

	uint16_t d02 = 0;
	uint16_t d12 = 0;
	uint16_t d22 = 0;

	uint16_t d03 = 0;
	uint16_t d13 = 0;
	uint16_t d23 = 0;

	uint16_t d04 = 0;
	uint16_t d14 = 0;
	uint16_t d24 = 0;

	uint16_t sma01 = 0;
	uint16_t sma11 = 0;
	uint16_t sma21 = 0;
	uint16_t smi01 = 0;
	uint16_t smi11 = 0;
	uint16_t smi21 = 0;

	uint16_t sma02 = 0;
	uint16_t sma12 = 0;
	uint16_t sma22 = 0;
	uint16_t smi02 = 0;
	uint16_t smi12 = 0;
	uint16_t smi22 = 0;

	uint16_t sma03 = 0;
	uint16_t sma13 = 0;
	uint16_t sma23 = 0;
	uint16_t smi03 = 0;
	uint16_t smi13 = 0;
	uint16_t smi23 = 0;

	uint16_t sma04 = 0;
	uint16_t sma14 = 0;
	uint16_t sma24 = 0;
	uint16_t smi04 = 0;
	uint16_t smi14 = 0;
	uint16_t smi24 = 0;

	uint16_t _min0 = 1023;
	uint16_t _max0 = 0;
	uint16_t _min1 = 1023;
	uint16_t _max1 = 0;
	uint16_t _min2 = 1023;
	uint16_t _max2 = 0;
	uint16_t _min3 = 1023;
	uint16_t _max3 = 0;

	uint16_t sma1v = 0;
	uint16_t smi1v = 0;
	uint16_t sma2v = 0;
	uint16_t smi2v = 0;
	uint16_t sma3v = 0;
	uint16_t smi3v = 0;
	uint16_t sma4v = 0;
	uint16_t smi4v = 0;

	uint16_t v0 = 0;
	uint16_t v1 = 0;
	uint16_t v2 = 0;
	uint16_t v3 = 0;
	
	adc_init();        	//Setup the ADC
	USART_init();      	//Setup the USART

	do {
		
		// Читаем
		a0 = read_adc(0);
		a1 = read_adc(1);
		a2 = read_adc(2);
		a3 = read_adc(3);

		// Фильтруем
		d01 = d11;
		d11 = d21;
		d21 = lowpass(d21, a0, VA);
		
		d02 = d12;
		d12 = d22;
		d22 = lowpass(d22, a1, VB);
		
		d03 = d13;
		d13 = d23;
		d23 = lowpass(d23, a2, VC);
		
		d04 = d14;
		d14 = d24;
		d24 = lowpass(d24, a3, VD);
		
		// Ищем min max
		if (d01<d11 && d11>d21)
		_max0 = d11;
		else if (d01>d11 && d11<d21)
		_min0 = d11;

		if (d02<d12 && d12>d22)
		_max1 = d12;
		else if (d02>d12 && d12<d22)
		_min1 = d12;

		if (d03<d13 && d13>d23)
		_max2 = d13;
		else if (d03>d13 && d13<d23)
		_min2 = d13;

		if (d04<d14 && d14>d24)
		_max3 = d14;
		else if (d04>d14 && d14<d24)
		_min3 = d14;

		// Фильтруем min max
		
		sma01 = sma11;
		sma11 = sma21;
		sma21 = _max0;
		smi01 = smi11;
		smi11 = smi21;
		smi21 = _min0;

		sma02 = sma12;
		sma12 = sma22;
		sma22 = _max1;
		smi02 = smi12;
		smi12 = smi22;
		smi22 = _min1;

		sma03 = sma13;
		sma13 = sma23;
		sma23 = _max2;
		smi03 = smi13;
		smi13 = smi23;
		smi23 = _min2;

		sma04 = sma14;
		sma14 = sma24;
		sma24 = _max3;
		smi04 = smi14;
		smi14 = smi24;
		smi24 = _min3;

		sma1v = (sma01+sma11+sma21)/3;
		smi1v = (smi01+smi11+smi21)/3;

		sma2v = (sma02+sma12+sma22)/3;
		smi2v = (smi02+smi12+smi22)/3;

		sma3v = (sma03+sma13+sma23)/3;
		smi3v = (smi03+smi13+smi23)/3;

		sma4v = (sma04+sma14+sma24)/3;
		smi4v = (smi04+smi14+smi24)/3;
		// Высчитываем амплитуду

		v0 = sma1v - smi1v;
		v1 = sma2v - smi2v;//(sma02-smi02+sma12-smi12+sma22-smi22)/3;
		v2 = sma3v - smi3v;//(sma03-smi03+sma13-smi13+sma23-smi23)/3;
		v3 = sma4v - smi4v;//(sma04-smi04+sma14-smi14+sma24-smi24)/3;
		
		// Оотправяем данные

		USART_send(ID+'0');
		USART_send(' ');
		ULongToStr(v0);
		USART_send(' ');
		ULongToStr(v1);
		USART_send(' ');
		ULongToStr(v2);
		USART_send(' ');
		ULongToStr(v3);
		USART_send('\r');
		USART_send('\n');                	//This two lines are to tell to the terminal to change line
		
	} while(1);

	return 0;
}

void adc_init(void){
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));  //16Mhz/128 = 125Khz the ADC reference clock
	ADMUX |= (1<<REFS0);                			//Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN);                			//Turn on ADC
	ADCSRA |= (1<<ADSC);                			//Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}

uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;                    	//Clear the older channel that was read
	ADMUX |= channel;                	//Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);               //Starts a new conversion
	while(ADCSRA & (1<<ADSC));         //Wait until the conversion is done
	return ADCW;                   	//Returns the ADC value of the chosen channel
}

void USART_init(void){

	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
}

void USART_send( unsigned char data){

	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;

}

void USART_putstring(char* StringPtr){

	while(*StringPtr != 0x00){
		USART_send(*StringPtr);
	StringPtr++;}
}

uint16_t lowpass(uint16_t prev, uint16_t cur, float a)
{
	return prev + a * (cur*1.0 - prev*1.0); // the best
}


void ULongToStr(uint16_t Value)
{
	static uint16_t powers[] = {10, 100, 1000};

	char digit = '0';
	uint16_t power = powers[2];
	while (Value >= power)
	{
		Value -= power;
		digit++;
	}
	USART_send(digit);

	digit = '0';
	power = powers[1];
	while (Value >= power)
	{
		Value -= power;
		digit++;
	}
	USART_send(digit);

	digit = '0';
	power = powers[0];
	while (Value >= power)
	{
		Value -= power;
		digit++;
	}
	USART_send(digit);
	
	USART_send((char) Value+'0');
}