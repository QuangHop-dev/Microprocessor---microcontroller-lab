#include"reg51.h"
#include"XPT2046.h"

/*
0xFF30CF: 1
0xFF18E7: 2
*/

sbit button = P3^3;
#define Display P0
unsigned char code Code7segCatot[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
                                     0x7d, 0x07, 0x7f, 0x6f, 0x77};
                           
unsigned char Time[6] = {0x10, 0x01, 0x16, 0x24, 0x10, 0x07};

unsigned char irKey = 0;
unsigned long bitPattern = 0;
unsigned long newKey = 0;
unsigned long pre_newKey = 0;
unsigned char timerValue;
unsigned char msCount = 0;

uint temp,count;
char pulseCount = 0;
void Timer0_Init(void);
unsigned char Decode_IRKey(unsigned char key);
void Timer1_Init(void);
void delay(int time);
void HienThiSoChay(int demA, int demB);

sbit led1=P2^0;
sbit led2=P2^2;
sbit led3=P2^4;
void main(void){
	P2 = 0xff;
	Timer0_Init();
	EA = 1; 
	while (1)
    {
				if(count==50)
				{
					count=0; 
				temp = Read_AD_Data(0xA4);		
				}
				count++;
				if(temp<=10)
					led2=0;
				else
					led2=1;
        if ((newKey != 0) && (pre_newKey != newKey)) 
        {
            irKey = Decode_IRKey(newKey);  
            
            switch (irKey) {
                case 1:  
                    led1 = 1;  
                    break;
                case 2:  
                    led1 = 0;  
                default:
                    break;
            }
        }
        pre_newKey = newKey;
				if(button == 1)
            delay(200);
            if(button == 0)
                HienThiSoChay(5,0);
	} 
}

void HienThiSoChay(int demA, int demB) {
  int tanso;
  int dem;
  for (dem = demA; dem >= demB; dem--) {
 
      Display = Code7segCatot[dem];
      delay(100000);            

  }
    led3 = ~led3;
} 

void delay(int time)
{
    while(--time);
}

unsigned char Decode_IRKey(unsigned char key){
	unsigned char returnValue = 0;
	switch (key)
	{
		case 0xFF30CF:
			returnValue = 1;
			break;
		case 0xFF18E7:
			returnValue = 2;
			break; 
	}
	return returnValue;
}

void timer0_isr() interrupt 1
{
	if (msCount < 50)
		msCount++;
	TH0 = 0xFC; 
	TL0 = 0x67;
} 
void externalIntr0_ISR() interrupt 0
{
	timerValue = msCount;
	msCount = 0;
	TH0 = 0xFC; 
	TL0 = 0x67;
	pulseCount++;
	if ((timerValue >= 50 )) 
	{
		pulseCount = -2; 
		bitPattern = 0;
	}
	else if ((pulseCount >= 0 ) && (pulseCount < 32)) 
	{
		if (timerValue >= 2 ) 
			bitPattern |= (unsigned long)1 << (31 - pulseCount);
	}
	else if (pulseCount >= 32 ) 
	{ 
		newKey = bitPattern; 
		pulseCount = 0;
	}
}
void Timer0_Init(){
	TMOD |= 0x01; 
	TH0 = 0xFC; 
	TL0 = 0x67;
	TR0 = 1; 
	ET0 = 1; 
	IT0 = 1; 
	EX0 = 1; 
}