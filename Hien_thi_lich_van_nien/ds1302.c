#include"ds1302.h"

uchar code READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d}; 
uchar code WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};

uchar TIME[7] = {0x00, 0x03, 0x16, 0x07, 0x10, 0x24, 0x24};

/*******************************************************************************
* 
*******************************************************************************/

void Ds1302Write(uchar addr, uchar dat)
{    
	uchar n;

    EA = 0;
	RST = 0;
	_nop_();

	SCLK = 0;
	_nop_();
	RST = 1; 
	_nop_();

	for (n=0; n<8; n++)
	{
		DSIO = addr & 0x01;
		addr >>= 1;
		SCLK = 1;
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;
	_nop_();
    EA = 1;
}

/*******************************************************************************
* 
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{   
    uchar n,dat,dat1;
    
    EA =0;
	RST = 0;
	_nop_();

	SCLK = 0;
	_nop_();
	RST = 1;
	_nop_();

	for(n=0; n<8; n++)
	{
		DSIO = addr & 0x01;
		addr >>= 1;
		SCLK = 1;
		_nop_();
		SCLK = 0;
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)
	{
		dat1 = DSIO;
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;
		_nop_();
	}

	RST = 0;
	_nop_();	
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
    EA = 1;
	return dat;	
}

/*******************************************************************************
*
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0X00);		 
	for (n=0; n<7; n++)
	{
		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		 
}

/*******************************************************************************
* 
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}


