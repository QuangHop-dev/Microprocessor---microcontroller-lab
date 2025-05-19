/*
0xFFA25D: CH-
0xFF629D: CH
0xFFE21D: CH+
0xFF22DD: PREV
0xFF02FD: NEXT
0xFFC23D: PLAY/PAUSE
0xFFE01F: VOL-
0xFFA857: VOL+
0xFF906F: EQ
0xFF6897: 0
0xFF9867: 100+
0xFFB04F: 200+
0xFF30CF: 1
0xFF18E7: 2
0xFF7A85: 3
0xFF10EF: 4
0xFF38C7:5
0xFF5AA5: 6
0xFF4280: 7
0xFF4AB5: 8
0xFF52AD: 9
*/
#include <REG51.H>
unsigned char irKey = 0;
unsigned long bitPattern = 0;
unsigned long newKey = 0;
unsigned long pre_newKey = 0;
unsigned char timerValue;
unsigned char msCount = 0;
char pulseCount = 0;
void Timer0_Init(void);
unsigned char Decode_IRKey(unsigned char key);
sbit led1=P2^0;
sbit led2=P2^2;
sbit led3=P2^4;
void main(){
	P2 = 0xEA;
	Timer0_Init();
	EA = 1; // Enable the Global Interrupt bit
	while (1)
    {
        // N?u nh?n du?c phím m?i và phím dó khác v?i phím tru?c dó
        if ((newKey != 0) && (pre_newKey != newKey)) 
        {
            irKey = Decode_IRKey(newKey);  // Gi?i mã phím di?u khi?n h?ng ngo?i
            
            // Ði?u khi?n 3 LED theo mã phím
            switch (irKey) {
                case 1:  // Ði?u khi?n LED 1 (P2.0)
                    led1 = 1;  // Ð?o bit P2.0 (B?t/T?t LED 1)
                    break;
                case 2:  // Ði?u khi?n LED 2 (P2.1)
                    led2 = 1;  // Ð?o bit P2.1 (B?t/T?t LED 2)
                    break;
                case 3:  // Ði?u khi?n LED 3 (P2.2)
                    led3 = 1;  // Ð?o bit P2.2 (B?t/T?t LED 3)
                    break;
								case 4:  // Ði?u khi?n LED 1 (P2.0)
                    led1 = 0;  // Ð?o bit P2.0 (B?t/T?t LED 1)
                    break;
                case 5:  // Ði?u khi?n LED 2 (P2.1)
                    led2 = 0;  // Ð?o bit P2.1 (B?t/T?t LED 2)
                    break;
                case 6:  // Ði?u khi?n LED 3 (P2.2)
                    led3 = 0;  // Ð?o bit P2.2 (B?t/T?t LED 3)
                    break;
                default:
                    // Không làm gì n?u nh?n mã phím không h?p l?
                    break;
            }
        }
        pre_newKey = newKey;  // Luu l?i mã phím hi?n t?i
	} 
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
		case 0xFF7A85: 
			returnValue = 3; 
			break; 
		case 0xFF10EF: 
			returnValue = 4 ; 
			break; 
		case 0xFF38C7: 
			returnValue = 5; 
			break; 
		case 0xFF5AA5: 
			returnValue = 6; 
			break; 
	}
	return returnValue;
}
void timer0_isr() interrupt 1
{
	if (msCount < 50)
		msCount++; 
	TH0 = 0xFC; // Reload the timer value for 1ms Delay 
	TL0 = 0x67;
} 
void externalIntr0_ISR() interrupt 0
{
	timerValue = msCount;
	msCount = 0;
	TH0 = 0xFC; // Reload the timer value for 1ms Delay
	TL0 = 0x67;
	pulseCount++;
	if ((timerValue >= 50 )) // If the pulse width is greater than 50ms, this will mark S0F
	{
		pulseCount = -2; // First 2 counts needs to be skipped hence pulse count is set to -2
		bitPattern = 0;
	}
	else if ((pulseCount >= 0 ) && (pulseCount < 32)) // Accumulate the bit values between 0-31.
	{
		if (timerValue >= 2 ) // pulse width greater than 2ms is considered as LOGIC1
			bitPattern |= (unsigned long)1 << (31 - pulseCount);
	}
	else if (pulseCount >= 32 ) // This will mark the End of frame as 32 pulses are received
	{ 
		newKey = bitPattern; // Copy the newkey(patter) and set the pulse count to 0
		pulseCount = 0;
	}
}
void Timer0_Init(){
	TMOD |= 0x01; // Timer0 MODE1(16-Bit timer)
	TH0 = 0xFC; // Timer value for 1ms at 11.0592Mhz clock
	TL0 = 0x67;
	TR0 = 1; // Start the Timer
	ET0 = 1; // Enable the Timer Interrupt
	IT0 = 1; // Configure INTO falling edge Interrupt
	EX0 = 1; // Enable the INTO External Interrupt
}