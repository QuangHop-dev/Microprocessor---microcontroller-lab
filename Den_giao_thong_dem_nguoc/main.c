/*LE HONG PHONG - 22119210*/
#include <AT89X52.h>

sbit SW_chedo = P3 ^ 2; /*chan nhan tin hieu chuyen che do*/
sbit SW = P3 ^ 3;/*chan nhan tin hieu chuyen huong*/

#define Display P0 /*xuat du lieu led 7 doan*/
#define chonLED P2 /*P2.4,P2.3,P2.2 dieukhien 8 led 7 doan*/

int chedo = 1;
int chuyenhuong = 1;
int chuyenVang = 0;

/*du lien hien thi so*/
unsigned char code Code7segCatot[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
                                      0x7d, 0x07, 0x7f, 0x6f, 0x77};
/*du lieu trang thai den*/
/*4 trang thai*/
/*do0 sang, vang0 tat, xanh0 tat, do1 tat, vang1 tat, xanh1 sang*/
/*do0 sang, vang0 tat, xanh0 tat, do1 tat, vang1 sang, xanh1 tat*/
/*do0 tat, vang0 tat, xanh0 sang, do1 sang, vang1 tat, xanh1 tat*/
/*do0 tat, vang0 sang, xanh0 tat, do1 sang, vang1 tat, xanh1 tat*/
unsigned char code StatusLED[] = {0x81, 0x82, 0x24, 0x44};

/*du lieu chan chon hien thi led P2 = {0}{0}{0}{P2.4}_{P2.3}{P2.2}{0}{0}*/ 
unsigned char code BNsochay[] = {0x00, 0x18, 0x04, 0x1c},
              DTsochay[] = {0x18, 0x00, 0x1c, 0x04};

void delay_ms(unsigned int time) {
  /* f = 12M hz*/
  /*f vào timer 1 = 12M/2 = 6M*/
  /*T = 1/6M s*/
  /* 6000 --> 1ms*/
  /*59536--> 65535 -> 0 = 1ms*/
  while (time--) {
    TMOD = 0x01;
    TH1 = 0xE8;
    TL1 = 0x90;
    TR1 = 1;
    while (!TF1)
      ;
    TF1 = 0;
  }
}
void HienThiDen(char Status) {
	
  int i;
  P3_6 = 0; /*clock canh len*/
  P3_5 = 0;/*chan chot du lieu*/
	/*74hc595 nhan du lieu noi tiep khi xung canh lenh SH_CP tac dong,
	moi xung canh len la nhan 1 bit
	xuat du lieu khi xung canh lenh ST_CP tac dong*/
  for (i = 0; i < 8; i++) {
    P3_4 = Status>>7;
		Status <<=1;
    delay_ms(1);
    P3_6 = 1;
    delay_ms(1);
    P3_6 = 0;
  }
  P3_5 = 1;
	
}
/*chi ho tro so 2 chu so*/
void HienThiSoChay(char *Huong, int demA, int demB) {
  int tanso;
  int dem;
  for (dem = demA; dem >= demB; dem--) {
    if (chedo == 1) {
      return;
    }
    for (tanso = 0; tanso < 60; tanso++) {
      chonLED = Huong[0];
      Display = Code7segCatot[dem % 10];
      delay_ms(3);
      chonLED = Huong[1];
      Display = Code7segCatot[(dem - 3) % 10];
      delay_ms(3);
      chonLED = Huong[2];
      Display = Code7segCatot[dem / 10];
      delay_ms(3);
      chonLED = Huong[3];
      Display = Code7segCatot[(dem - 3) / 10];
      delay_ms(3);
			#hien thi den led matrix
			Display = 0x00;
			delay_ms(3);			
    }
  }
}
/*chi ho tro so 2 chu so*/
void HienThiSoVang(char *Huong, int demA, int demB) {
  int tanso;
  int dem;
  for (dem = demA; dem >= demB; dem--) {
    if (chedo == 1) {
      return;
    }
    for (tanso = 0; tanso < 60; tanso++) {
      chonLED = Huong[0];
      Display = Code7segCatot[dem % 10];
      delay_ms(3);
      chonLED = Huong[1];
      Display = Code7segCatot[dem % 10];
      delay_ms(3);
      chonLED = Huong[2];
      Display = Code7segCatot[dem / 10];
      delay_ms(3);
      chonLED = Huong[3];
      Display = Code7segCatot[dem / 10];
      delay_ms(3);
			#hien thi den led matrix
			Display = 0x00;
			delay_ms(3);
    }
  }
}
void Traffic_Auto(void) {
  // delay10s
  HienThiDen(StatusLED[0]);
  HienThiSoChay(BNsochay, 30, 3);
  // delay3s
  HienThiDen(StatusLED[1]);
  HienThiSoVang(BNsochay, 3, 0);
  // delay10s
  HienThiDen(StatusLED[2]);
  HienThiSoChay(DTsochay, 30, 3);
  // delay3s
  HienThiDen(StatusLED[3]);
  HienThiSoVang(DTsochay, 3, 0);
}
void Interrupt_Timer0(void) {
  /* f = 12M hz*/
  /*f vào timer 0 = 12M/12 = 1M*/
  /*T = 1/6M s*/
  /* 1000 --> 1ms*/
  /*64536--> 65535 -> 0 = 1ms*/
  /* 16 bits timer */
  TMOD = 0x01;
  /* setup 1ms for overforlow */
  TH0 = 0xFC;
  TL0 = 0x18;
  /* Enable interrupt */
  ET0 = 1;
  EA = 1;
  /* Enable timer0 */
  TR0 = 1;
}

void ISR_TIMER(void) interrupt 1 { 
  if (ET0 == 1) {
    if (SW_chedo == 0) {
      delay_ms(100);
      if (SW_chedo == 1) {
        chedo++;
      }
      if (chedo > 1)
        chedo = 0;
    }
    /*setup 1ms for overforlow*/
    TH0 = 0xFC;
    TL0 = 0x18;
  }
}

void main(void) {
  Interrupt_Timer0();/*1ms interrupt se kiem tra trang thai chan SW_chedo*/
  while (1) {
    if (chedo) {
      P0 = 0x00;
      if (SW == 0) {
        delay_ms(100);
        if (SW == 1) {
          chuyenhuong++;
        }
        if (chuyenhuong > 1)
          chuyenhuong = 0;
      }
      if (chuyenhuong) {
				if(chuyenVang){
					HienThiDen(StatusLED[3]);
					delay_ms(2000);
				}
        HienThiDen(StatusLED[0]);
				chuyenVang = 0;
      } else {
				if(!chuyenVang){
					HienThiDen(StatusLED[1]);
					delay_ms(2000);
				}
        HienThiDen(StatusLED[2]);
				chuyenVang = 1;
      }
    } else
      Traffic_Auto();
  }
}
