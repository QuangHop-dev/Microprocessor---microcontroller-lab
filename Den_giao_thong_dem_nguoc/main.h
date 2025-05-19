#include <AT89X52.h>
/*#define StatusTraffic /*do0, do1, vang0, vang1, xanh0, xanh1*/
sbit SW_chedo = P3^2;
sbit SW = P3^3;

#define Display P0 /*xuat du lieu led 7 doan*/
#define chonLED P2 /*P2.4,P2.3,P2.2 dieukhien 8 led 7 doan*/
