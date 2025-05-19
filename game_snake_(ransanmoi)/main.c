#include <regx52.h>  
#include <stdlib.h>  		// Thu vi?n h? tr? c�c h�m ng?u nhi�n v� ti?n �ch

/* �?nh nghia d? kh� */
#define TIMER0_COUNTER_MAX 10  //Gi?ih?n Timer0 d? di?u khi?n t?c d? tr� choi
#define SNAKE_MAX_LENGTH 64    // �? d�i t?i da c?a con r?n
#define GAME_OVER 1            // Tr?ng th�i tr� choi k?t th�c
#define GAME_PLAYING 0         // Tr?ng th�i tr� choi dang ch?y

/* �?nh nghia c�c ch�n k?t n?i c?a IC 74HC595 */
sbit CLOCK = P3^6;  		   // Ch�n CLOCK di?u khi?n t�n hi?u d?ng h?
sbit LATCH = P3^5;  		   // Ch�n LATCH ch?t d? li?u
sbit DATA = P3^4;   		   // Ch�n DATA truy?n d? li?u

/* Bi?n d?m cho c�c b? d?m th?i gian */
unsigned char timer2_counter = 0;  		// Bi?n d?m cho Timer2
unsigned char timer0_counter = 0;  		// Bi?n d?m cho Timer0

/* B? d?m hi?n th? 8x8 LED */
unsigned char xdata displayBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
// Luu tr?ng th�i c?a t?ng h�ng LED

/* Ma tr?n gi� tr? c?a b�n ph�m 4x4 */
char code keypad[17] = {
    10, 'u', 10, 10,    // H�ng 1
    'l', 's', 'r', 10,  // H�ng 2
    10, 'd', 10, 10,    // H�ng 3
    10, 10, 10, 10,     // H�ng 4
    10                  // Ph?n du kh�ng d�ng
};

/* C?u tr�c bi?u di?n 1 di?m tr�n m�n h�nh */
typedef struct {
    unsigned char x;  	// T?a d? x
    unsigned char y;  	// T?a d? y
} Point;

/* �?i tu?ng trong tr� choi */
Point xdata snake[SNAKE_MAX_LENGTH];  // Danh s�ch c�c di?m t?o th�nh con r?n
Point apple;                          // V? tr� c?a qu? t�o
unsigned char snakeLength = 0;        // Chi?u d�i c?a con r?n
unsigned char snakeDirection = 0;     // Hu?ng di chuy?n c?a con r?n
bit gameState = GAME_OVER;            // Tr?ng th�i tr� choi

/* Khai b�o c�c h�m tr� choi */
void Snake_Display();                 // Hi?n th? con r?n
void Snake_Step();                    // C?p nh?t tr?ng th�i con r?n
unsigned char isSnakeHitItself();     // Ki?m tra r?n t? va ch?m
void Apple_Create();                  // T?o qu? t�o m?i
void Apple_Display();                 // Hi?n th? qu? t�o
void Display_Clear();                 // X�a b? d?m hi?n th?
void Display_Point(Point);            // Hi?n th? 1 di?m tr�n LED
void Display_Point2(unsigned x, unsigned y);  // Hi?n th? 1 di?m v?i t?a d? 
void Keypad_Read();                   // �?c b�n ph�m
char Keypad_Check();                  // Ki?m tra tr?ng th�i b�n ph�m
char Keypad_Position(unsigned short value);  // L?y v? tr� nh?n tr�n b�n ph�m

/* C�c h�m kh�c */
void Matrix_StartDisplay();           // B?t d?u hi?n th? LED ma tr?n
void Game_Init();                     // Kh?i t?o tr� choi
void Game_Start();                    // B?t d?u tr� choi
void Game_Stop();                     // D?ng tr� choi

void main () {
    // Bi?n d�ng d? x? l� b�n ph�m
    unsigned char counter2 = 0;
    char key_clicked = 0;  			// Ph�m v?a nh?n
    char pre_key_clicked = 0;  		// Ph�m nh?n tru?c d�

    /* Thi?t l?p ban d?u */
    P1 = 0xff;  				// Thi?t l?p c�c ch�n c?a Port 1 l�m d?u v�o
    P2 = 0;     				// X�a d? li?u Port 2
    P0 = 0;     				// X�a d? li?u Port 0
    EA = 1;     				// Cho ph�p ng?t to�n c?c
    LATCH = 0;  				// Ch�n LATCH ? m?c th?p
    DATA = 0;   				// Ch�n DATA ? m?c th?p
    CLOCK = 0;  				// Ch�n CLOCK ? m?c th?p

    /* B?t d?u hi?n th? LED ma tr?n */
    Matrix_StartDisplay();

    /* Kh?i t?o tr� choi */
    Game_Init();

    /* V�ng l?p ch�nh d? x? l� b�n ph�m */
    while(1) {
        key_clicked = keypad[Keypad_Check()];  	// L?y ph�m nh?n hi?n t?i
        if((pre_key_clicked != key_clicked) && (key_clicked != 10)) {
            // Khi tr� choi chua b?t d?u v� nh?n ph�m 's'
            if ((gameState == GAME_OVER) && (key_clicked == 's')) {
                Game_Init();  				// Kh?i t?o l?i tr� choi
                Game_Start(); 				// B?t d?u tr� choi
            }
            else if (gameState == GAME_PLAYING) {
                snakeDirection = key_clicked;  	
// C?p nh?t hu?ng di chuy?n c?a r?n
            }
        }
        pre_key_clicked = key_clicked;  		// Luu ph�m nh?n tru?c d�
    }
}
void Snake_Display(){
    unsigned char counter2 = 0;
    Display_Clear();			// X�a m�n h�nh hi?n th? (c�c di?m tru?c d�)
    Apple_Display();			// Hi?n th? v? tr� c?a t�o

    // Duy?t qua t?t c? c�c ph?n c?a r?n v� hi?n th? ch�ng
    for(counter2 = 0; counter2 < snakeLength; counter2++){
        // Hi?n th? m?i ph?n c?a r?n
        Display_Point(snake[counter2]);
    }
}

void Snake_Step(){
    unsigned char counter2 = 0;
    // Luu v? tr� du�i r?n
    Point snake_tail;
    snake_tail.x = snake[snakeLength - 1].x;
    snake_tail.y = snake[snakeLength - 1].y;

    switch (snakeDirection)
    {
        // Di chuy?n r?n sang ph?i
        case 'r':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? du�i v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n sang ph?i
            snake[0].x = snake[0].x + 1;
            if(snake[0].x == 8){
                // N?u r?n va v�o c?nh ph?i, dua d?u r?n v? ph�a tr�i
                snake[0].x = 0;
            }
            break;

        // Di chuy?n r?n sang tr�i
        case 'l':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? du�i v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n sang tr�i
            snake[0].x--;
            if(snake[0].x == 255){
                // N?u r?n va v�o c?nh tr�i, dua d?u r?n v? ph�a ph?i
                snake[0].x = 7;
            }
            break;

        // Di chuy?n r?n l�n tr�n
        case 'u':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? du�i v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n l�n tr�n
            snake[0].y++;
            if(snake[0].y == 8){
                // N?u r?n va v�o c?nh tr�n, dua d?u r?n v? ph�a du?i
                snake[0].y = 0;
            }
            break;

        // Di chuy?n r?n xu?ng du?i
        case 'd':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? du�i v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n xu?ng du?i
            snake[0].y--;
            if(snake[0].y == 255){
                // N?u r?n va v�o c?nh du?i, dua d?u r?n v? ph�a tr�n
                snake[0].y = 7;
            }
            break;

        default:
            break;
    }

    // Ki?m tra n?u r?n an du?c t�o
    if((snake[0].x == apple.x) && (snake[0].y == apple.y)){
        // Tang chi?u d�i c?a r?n: th�m ph?n du�i m?i v�o v? tr� c?a du�i cu
        snake[snakeLength].x = snake_tail.x;
        snake[snakeLength].y = snake_tail.y;
        snakeLength++;  			// Tang d? d�i r?n
        Apple_Create(); 			// T?o l?i t�o ? v? tr� m?i
    }

    // Ki?m tra n?u r?n d?ng ph?i ch�nh n�
    if(isSnakeHitItself()){
        Game_Stop();  			// D?ng tr� choi n?u r?n t? va v�o m�nh
    }

    // C?p nh?t hi?n th? c?a r?n tr�n m�n h�nh
    Snake_Display();
}

unsigned char isSnakeHitItself() {
    unsigned char returnValue = 0;
    unsigned char counter2 = 0;
    // Duy?t qua t?t c? c�c ph?n c?a r?n, b?t d?u t? ph?n th? hai 

    for(counter2 = 1; counter2 < snakeLength; counter2++){
        // Ki?m tra d?u r?n c� tr�ng v? tr� v?i ph?n n�o kh�c c?a r?n kh�ng
        returnValue = (snake[0].x == snake[counter2].x) && (snake[0].y == snake[counter2].y);
        if (returnValue) break; // N?u c� va ch?m, d?ng v�ng l?p
    }
    return returnValue; // Tr? v? 1 n?u r?n va v�o ch�nh n�, ngu?c l?i v? 0
}
void Apple_Display(){		// Hi?n th? t�o t?i v? tr� hi?n t?i c?a n�
    Display_Point(apple);
}

void Apple_Create(){
    unsigned char check = 1;
    unsigned char counter2 = 0;
    // L?p l?i cho d?n khi t?o m?t v? tr� t�o kh�ng tr�ng v?i ph?n n�o c?a r?n
    do
    {
        // T?o m?t v? tr� ng?u nhi�n cho t�o trong ph?m vi t? 0 d?n 7 
        apple.x = ((unsigned int) rand()) % 8;
        apple.y = ((unsigned int) rand()) % 8;

        // Ki?m tra xem t�o c� b? tr�ng v?i b?t k? ph?n n�o c?a r?n kh�ng
        for (counter2 = 0; counter2 < snakeLength; counter2++)
        {
            check = (apple.x == snake[counter2].x) && (apple.y == snake[counter2].y);
            if(check) break; // N?u c� tr�ng, tho�t kh?i v�ng l?p ki?m tra
        }
    }
    // Ti?p t?c n?u t�o tr�ng v? tr� v?i r?n
    while (check);
}

void Display_Clear(){
    unsigned char counter2 = 0;
    // Duy?t qua t?t c? c�c v? tr� tr�n m�n h�nh 
    for (counter2 = 0; counter2 < 8; counter2++) {
        // �?t t?t c? c�c di?m tr�n m�n h�nh v? 0 (t?c l� l�m m?i m�n h�nh)
        displayBuffer[counter2] = 0;
    }
}
/* Hi?n th? m?t di?m tr�n m�n h�nh */
void Display_Point(Point point){
    unsigned char x = point.x;
    unsigned char y = point.y;
    // C?p nh?t b? nh? d?m hi?n th? (displayBuffer) b?ng c�ch d?t bit t?i v? tr� (x, y) th�nh 1
    displayBuffer[7 - y] = displayBuffer[7 - y] | (1 << (7 - x));
}

/* Ki?m tra v? tr� ph�m tr�n b�n ph�m ma tr?n */
char Keypad_Check(){
    unsigned short clickPos = 0;
    // C�i d?t P1 v?i c�c gi� tr? d? qu�t c�c c?t c?a b�n ph�m
    P1 = 0xef;  // Ch?n c?t 1
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng th�i c?a c?t 1
    P1 = 0xdf;  // Ch?n c?t 2
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng th�i c?a c?t 2
    P1 = 0xbf;  // Ch?n c?t 3
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng th�i c?a c?t 3
    P1 = 0x7f;  // Ch?n c?t 4
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng th�i c?a c?t 4
    // Tr? v? v? tr� c?a ph�m d� nh?n
    return Keypad_Position(clickPos);
}
/* X�c d?nh v? tr� ph�m trong b�n ph�m ma tr?n */
char Keypad_Position(unsigned short value){
    char returnvalue = 16;
    // Ki?m tra gi� tr? c?a `value` (l� tr?ng th�i c?a b�n ph�m sau khi qu�t)
    switch (value & 0xff)
    {
        case 0xf7: returnvalue = 0; break;
        case 0xfb: returnvalue = 1; break;
        case 0xfd: returnvalue = 2; break;
        case 0xfe: returnvalue = 3; break;
        case 0x7f: returnvalue = 4; break;
        case 0xbf: returnvalue = 5; break;
        case 0xdf: returnvalue = 6; break;
        case 0xef: returnvalue = 7; break;
        default:
            value = value >> 8;  // D?ch 8 bit ki?m tra ph?n tr�n c?a `value`
            switch (value & 0xff)
            {
                case 0xf7: returnvalue = 8; break;
                case 0xfb: returnvalue = 9; break;
                case 0xfd: returnvalue = 10; break;
                case 0xfe: returnvalue = 11; break;
                case 0x7f: returnvalue = 12; break;
                case 0xbf: returnvalue = 13; break;
                case 0xdf: returnvalue = 14; break;
                case 0xef: returnvalue = 15; break;
            }
            break;
    }
    return returnvalue;
}
void Timer0_Isr(void) interrupt 1
{
    TL0 = 0x00;  		// �?t l?i gi� tr? th?p c?a b? d?m Timer0
    TH0 = 0x04;  		// �?t l?i gi� tr? cao c?a b? d?m Timer0
    timer0_counter++;  	// Tang gi� tr? c?a bi?n d?m Timer0
    if(timer0_counter == TIMER0_COUNTER_MAX){
        timer0_counter = 0;  // N?u gi� tr? d?m d?t gi?i h?n, reset l?i b? d?m
        Snake_Step();  	// G?i h�m x? l� bu?c di chuy?n c?a r?n
    }
    // �?t l?i gi� tr? c?a Timer0 d? ti?p t?c d?m
}
void Game_Init(){
    snakeLength = 3;  			// Kh?i t?o d? d�i r?n
    apple.x = 5;					/* V? tr� c?a qu? t�o */
    apple.y = 5;
    
    snake[2].x = 3;				/* V? tr� c?a c�c ph?n r?n */
    snake[2].y = 3;
    snake[1].x = 4;
    snake[1].y = 3;
    snake[0].x = 5;
    snake[0].y = 3;
    /* Hu?ng di chuy?n ban d?u */
    snakeDirection = 'r';  			// 'r' l� hu?ng ph?i
    gameState = GAME_OVER;  	// Tr?ng th�i tr� choi b?t d?u l� GAME_OVER
    Snake_Display();  			// Hi?n th? r?n
}
void Game_Start(void)
{
    TMOD &= 0xF0;  		// Thi?t l?p ch? d? ho?t d?ng c?a Timer0 
    TMOD |= 0x01;  		// Ch?n ch? d? 16-bit cho Timer0
    TL0 = 0x00;    		// �?t l?i gi� tr? th?p c?a Timer0
    TH0 = 0x04;    		// �?t l?i gi� tr? cao c?a Timer0
    TF0 = 0;       		// X�a c? ng?t TF0
    TR0 = 1;       		// B?t d?u Timer0

    ET0 = 1;       		// B?t ng?t Timer0

    // Tr?ng th�i tr� choi du?c chuy?n sang GAME_PLAYING
    gameState = GAME_PLAYING;
}

void Game_Stop(){
    TR0 = 0;  			 // D?ng Timer0
    gameState = GAME_OVER;  // �?t tr?ng th�i tr� choi l� GAME_OVER
}

void Timer2_Isr(void) interrupt 5
{
    TF2 = 0;  				// X�a c? ng?t Timer2
    DATA = timer2_counter == 0;  // Thi?t l?p DATA t�y thu?c v�o gi� tr? d?m
    CLOCK = 1;  				// C?p nh?t t�n hi?u CLOCK
    // Hi?n th? m?t gi� tr? t? displayBuffer tr�n c?ng P0
    P0 = ~displayBuffer[7 - timer2_counter];  
    LATCH = 1;  				// C?p nh?t t�n hi?u LATCH
    timer2_counter++;  		// Tang gi� tr? d?m c?a Timer2
    // Reset gi� tr? d?m khi d?t d?n 8
    if(timer2_counter == 8) timer2_counter = 0;  
    CLOCK = 0;  				// T?t t�n hi?u CLOCK
    LATCH = 0;  				// T?t t�n hi?u LATCH
}

void Matrix_StartDisplay(void)
{
    T2MOD = 0;       			// �?t l?i c�c thanh ghi di?u khi?n c?a Timer2
    T2CON = 0;       			// �?t l?i c�c thanh ghi c?u h�nh Timer2
    TL2 = 0xCD;      			// �?t gi� tr? th?p c?a Timer2
    TH2 = 0xF8;      			// �?t gi� tr? cao c?a Timer2
    RCAP2L = 0xCD;   			// �?t gi� tr? n?p l?i th?p c?a Timer2
    RCAP2H = 0xF8;   			// �?t gi� tr? n?p l?i cao c?a Timer2
    TR2 = 1;         			// B?t Timer2
    ET2 = 1;         			// B?t ng?t Timer2
}
