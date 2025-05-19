#include <regx52.h>  
#include <stdlib.h>  		// Thu vi?n h? tr? các hàm ng?u nhiên và ti?n ích

/* Ð?nh nghia d? khó */
#define TIMER0_COUNTER_MAX 10  //Gi?ih?n Timer0 d? di?u khi?n t?c d? trò choi
#define SNAKE_MAX_LENGTH 64    // Ð? dài t?i da c?a con r?n
#define GAME_OVER 1            // Tr?ng thái trò choi k?t thúc
#define GAME_PLAYING 0         // Tr?ng thái trò choi dang ch?y

/* Ð?nh nghia các chân k?t n?i c?a IC 74HC595 */
sbit CLOCK = P3^6;  		   // Chân CLOCK di?u khi?n tín hi?u d?ng h?
sbit LATCH = P3^5;  		   // Chân LATCH ch?t d? li?u
sbit DATA = P3^4;   		   // Chân DATA truy?n d? li?u

/* Bi?n d?m cho các b? d?m th?i gian */
unsigned char timer2_counter = 0;  		// Bi?n d?m cho Timer2
unsigned char timer0_counter = 0;  		// Bi?n d?m cho Timer0

/* B? d?m hi?n th? 8x8 LED */
unsigned char xdata displayBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};  
// Luu tr?ng thái c?a t?ng hàng LED

/* Ma tr?n giá tr? c?a bàn phím 4x4 */
char code keypad[17] = {
    10, 'u', 10, 10,    // Hàng 1
    'l', 's', 'r', 10,  // Hàng 2
    10, 'd', 10, 10,    // Hàng 3
    10, 10, 10, 10,     // Hàng 4
    10                  // Ph?n du không dùng
};

/* C?u trúc bi?u di?n 1 di?m trên màn hình */
typedef struct {
    unsigned char x;  	// T?a d? x
    unsigned char y;  	// T?a d? y
} Point;

/* Ð?i tu?ng trong trò choi */
Point xdata snake[SNAKE_MAX_LENGTH];  // Danh sách các di?m t?o thành con r?n
Point apple;                          // V? trí c?a qu? táo
unsigned char snakeLength = 0;        // Chi?u dài c?a con r?n
unsigned char snakeDirection = 0;     // Hu?ng di chuy?n c?a con r?n
bit gameState = GAME_OVER;            // Tr?ng thái trò choi

/* Khai báo các hàm trò choi */
void Snake_Display();                 // Hi?n th? con r?n
void Snake_Step();                    // C?p nh?t tr?ng thái con r?n
unsigned char isSnakeHitItself();     // Ki?m tra r?n t? va ch?m
void Apple_Create();                  // T?o qu? táo m?i
void Apple_Display();                 // Hi?n th? qu? táo
void Display_Clear();                 // Xóa b? d?m hi?n th?
void Display_Point(Point);            // Hi?n th? 1 di?m trên LED
void Display_Point2(unsigned x, unsigned y);  // Hi?n th? 1 di?m v?i t?a d? 
void Keypad_Read();                   // Ð?c bàn phím
char Keypad_Check();                  // Ki?m tra tr?ng thái bàn phím
char Keypad_Position(unsigned short value);  // L?y v? trí nh?n trên bàn phím

/* Các hàm khác */
void Matrix_StartDisplay();           // B?t d?u hi?n th? LED ma tr?n
void Game_Init();                     // Kh?i t?o trò choi
void Game_Start();                    // B?t d?u trò choi
void Game_Stop();                     // D?ng trò choi

void main () {
    // Bi?n dùng d? x? lý bàn phím
    unsigned char counter2 = 0;
    char key_clicked = 0;  			// Phím v?a nh?n
    char pre_key_clicked = 0;  		// Phím nh?n tru?c dó

    /* Thi?t l?p ban d?u */
    P1 = 0xff;  				// Thi?t l?p các chân c?a Port 1 làm d?u vào
    P2 = 0;     				// Xóa d? li?u Port 2
    P0 = 0;     				// Xóa d? li?u Port 0
    EA = 1;     				// Cho phép ng?t toàn c?c
    LATCH = 0;  				// Chân LATCH ? m?c th?p
    DATA = 0;   				// Chân DATA ? m?c th?p
    CLOCK = 0;  				// Chân CLOCK ? m?c th?p

    /* B?t d?u hi?n th? LED ma tr?n */
    Matrix_StartDisplay();

    /* Kh?i t?o trò choi */
    Game_Init();

    /* Vòng l?p chính d? x? lý bàn phím */
    while(1) {
        key_clicked = keypad[Keypad_Check()];  	// L?y phím nh?n hi?n t?i
        if((pre_key_clicked != key_clicked) && (key_clicked != 10)) {
            // Khi trò choi chua b?t d?u và nh?n phím 's'
            if ((gameState == GAME_OVER) && (key_clicked == 's')) {
                Game_Init();  				// Kh?i t?o l?i trò choi
                Game_Start(); 				// B?t d?u trò choi
            }
            else if (gameState == GAME_PLAYING) {
                snakeDirection = key_clicked;  	
// C?p nh?t hu?ng di chuy?n c?a r?n
            }
        }
        pre_key_clicked = key_clicked;  		// Luu phím nh?n tru?c dó
    }
}
void Snake_Display(){
    unsigned char counter2 = 0;
    Display_Clear();			// Xóa màn hình hi?n th? (các di?m tru?c dó)
    Apple_Display();			// Hi?n th? v? trí c?a táo

    // Duy?t qua t?t c? các ph?n c?a r?n và hi?n th? chúng
    for(counter2 = 0; counter2 < snakeLength; counter2++){
        // Hi?n th? m?i ph?n c?a r?n
        Display_Point(snake[counter2]);
    }
}

void Snake_Step(){
    unsigned char counter2 = 0;
    // Luu v? trí duôi r?n
    Point snake_tail;
    snake_tail.x = snake[snakeLength - 1].x;
    snake_tail.y = snake[snakeLength - 1].y;

    switch (snakeDirection)
    {
        // Di chuy?n r?n sang ph?i
        case 'r':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? duôi v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n sang ph?i
            snake[0].x = snake[0].x + 1;
            if(snake[0].x == 8){
                // N?u r?n va vào c?nh ph?i, dua d?u r?n v? phía trái
                snake[0].x = 0;
            }
            break;

        // Di chuy?n r?n sang trái
        case 'l':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? duôi v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n sang trái
            snake[0].x--;
            if(snake[0].x == 255){
                // N?u r?n va vào c?nh trái, dua d?u r?n v? phía ph?i
                snake[0].x = 7;
            }
            break;

        // Di chuy?n r?n lên trên
        case 'u':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? duôi v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n lên trên
            snake[0].y++;
            if(snake[0].y == 8){
                // N?u r?n va vào c?nh trên, dua d?u r?n v? phía du?i
                snake[0].y = 0;
            }
            break;

        // Di chuy?n r?n xu?ng du?i
        case 'd':
            for(counter2 = snakeLength - 1; counter2 > 0; counter2--){
                // Di chuy?n t?ng ph?n c?a r?n t? duôi v? d?u
                snake[counter2].x = snake[counter2 - 1].x;
                snake[counter2].y = snake[counter2 - 1].y;
            }
            // Di chuy?n d?u r?n xu?ng du?i
            snake[0].y--;
            if(snake[0].y == 255){
                // N?u r?n va vào c?nh du?i, dua d?u r?n v? phía trên
                snake[0].y = 7;
            }
            break;

        default:
            break;
    }

    // Ki?m tra n?u r?n an du?c táo
    if((snake[0].x == apple.x) && (snake[0].y == apple.y)){
        // Tang chi?u dài c?a r?n: thêm ph?n duôi m?i vào v? trí c?a duôi cu
        snake[snakeLength].x = snake_tail.x;
        snake[snakeLength].y = snake_tail.y;
        snakeLength++;  			// Tang d? dài r?n
        Apple_Create(); 			// T?o l?i táo ? v? trí m?i
    }

    // Ki?m tra n?u r?n d?ng ph?i chính nó
    if(isSnakeHitItself()){
        Game_Stop();  			// D?ng trò choi n?u r?n t? va vào mình
    }

    // C?p nh?t hi?n th? c?a r?n trên màn hình
    Snake_Display();
}

unsigned char isSnakeHitItself() {
    unsigned char returnValue = 0;
    unsigned char counter2 = 0;
    // Duy?t qua t?t c? các ph?n c?a r?n, b?t d?u t? ph?n th? hai 

    for(counter2 = 1; counter2 < snakeLength; counter2++){
        // Ki?m tra d?u r?n có trùng v? trí v?i ph?n nào khác c?a r?n không
        returnValue = (snake[0].x == snake[counter2].x) && (snake[0].y == snake[counter2].y);
        if (returnValue) break; // N?u có va ch?m, d?ng vòng l?p
    }
    return returnValue; // Tr? v? 1 n?u r?n va vào chính nó, ngu?c l?i v? 0
}
void Apple_Display(){		// Hi?n th? táo t?i v? trí hi?n t?i c?a nó
    Display_Point(apple);
}

void Apple_Create(){
    unsigned char check = 1;
    unsigned char counter2 = 0;
    // L?p l?i cho d?n khi t?o m?t v? trí táo không trùng v?i ph?n nào c?a r?n
    do
    {
        // T?o m?t v? trí ng?u nhiên cho táo trong ph?m vi t? 0 d?n 7 
        apple.x = ((unsigned int) rand()) % 8;
        apple.y = ((unsigned int) rand()) % 8;

        // Ki?m tra xem táo có b? trùng v?i b?t k? ph?n nào c?a r?n không
        for (counter2 = 0; counter2 < snakeLength; counter2++)
        {
            check = (apple.x == snake[counter2].x) && (apple.y == snake[counter2].y);
            if(check) break; // N?u có trùng, thoát kh?i vòng l?p ki?m tra
        }
    }
    // Ti?p t?c n?u táo trùng v? trí v?i r?n
    while (check);
}

void Display_Clear(){
    unsigned char counter2 = 0;
    // Duy?t qua t?t c? các v? trí trên màn hình 
    for (counter2 = 0; counter2 < 8; counter2++) {
        // Ð?t t?t c? các di?m trên màn hình v? 0 (t?c là làm m?i màn hình)
        displayBuffer[counter2] = 0;
    }
}
/* Hi?n th? m?t di?m trên màn hình */
void Display_Point(Point point){
    unsigned char x = point.x;
    unsigned char y = point.y;
    // C?p nh?t b? nh? d?m hi?n th? (displayBuffer) b?ng cách d?t bit t?i v? trí (x, y) thành 1
    displayBuffer[7 - y] = displayBuffer[7 - y] | (1 << (7 - x));
}

/* Ki?m tra v? trí phím trên bàn phím ma tr?n */
char Keypad_Check(){
    unsigned short clickPos = 0;
    // Cài d?t P1 v?i các giá tr? d? quét các c?t c?a bàn phím
    P1 = 0xef;  // Ch?n c?t 1
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng thái c?a c?t 1
    P1 = 0xdf;  // Ch?n c?t 2
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng thái c?a c?t 2
    P1 = 0xbf;  // Ch?n c?t 3
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng thái c?a c?t 3
    P1 = 0x7f;  // Ch?n c?t 4
    clickPos = (clickPos << 4) | (P1 & 0x0f); // Luu tr?ng thái c?a c?t 4
    // Tr? v? v? trí c?a phím dã nh?n
    return Keypad_Position(clickPos);
}
/* Xác d?nh v? trí phím trong bàn phím ma tr?n */
char Keypad_Position(unsigned short value){
    char returnvalue = 16;
    // Ki?m tra giá tr? c?a `value` (là tr?ng thái c?a bàn phím sau khi quét)
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
            value = value >> 8;  // D?ch 8 bit ki?m tra ph?n trên c?a `value`
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
    TL0 = 0x00;  		// Ð?t l?i giá tr? th?p c?a b? d?m Timer0
    TH0 = 0x04;  		// Ð?t l?i giá tr? cao c?a b? d?m Timer0
    timer0_counter++;  	// Tang giá tr? c?a bi?n d?m Timer0
    if(timer0_counter == TIMER0_COUNTER_MAX){
        timer0_counter = 0;  // N?u giá tr? d?m d?t gi?i h?n, reset l?i b? d?m
        Snake_Step();  	// G?i hàm x? lý bu?c di chuy?n c?a r?n
    }
    // Ð?t l?i giá tr? c?a Timer0 d? ti?p t?c d?m
}
void Game_Init(){
    snakeLength = 3;  			// Kh?i t?o d? dài r?n
    apple.x = 5;					/* V? trí c?a qu? táo */
    apple.y = 5;
    
    snake[2].x = 3;				/* V? trí c?a các ph?n r?n */
    snake[2].y = 3;
    snake[1].x = 4;
    snake[1].y = 3;
    snake[0].x = 5;
    snake[0].y = 3;
    /* Hu?ng di chuy?n ban d?u */
    snakeDirection = 'r';  			// 'r' là hu?ng ph?i
    gameState = GAME_OVER;  	// Tr?ng thái trò choi b?t d?u là GAME_OVER
    Snake_Display();  			// Hi?n th? r?n
}
void Game_Start(void)
{
    TMOD &= 0xF0;  		// Thi?t l?p ch? d? ho?t d?ng c?a Timer0 
    TMOD |= 0x01;  		// Ch?n ch? d? 16-bit cho Timer0
    TL0 = 0x00;    		// Ð?t l?i giá tr? th?p c?a Timer0
    TH0 = 0x04;    		// Ð?t l?i giá tr? cao c?a Timer0
    TF0 = 0;       		// Xóa c? ng?t TF0
    TR0 = 1;       		// B?t d?u Timer0

    ET0 = 1;       		// B?t ng?t Timer0

    // Tr?ng thái trò choi du?c chuy?n sang GAME_PLAYING
    gameState = GAME_PLAYING;
}

void Game_Stop(){
    TR0 = 0;  			 // D?ng Timer0
    gameState = GAME_OVER;  // Ð?t tr?ng thái trò choi là GAME_OVER
}

void Timer2_Isr(void) interrupt 5
{
    TF2 = 0;  				// Xóa c? ng?t Timer2
    DATA = timer2_counter == 0;  // Thi?t l?p DATA tùy thu?c vào giá tr? d?m
    CLOCK = 1;  				// C?p nh?t tín hi?u CLOCK
    // Hi?n th? m?t giá tr? t? displayBuffer trên c?ng P0
    P0 = ~displayBuffer[7 - timer2_counter];  
    LATCH = 1;  				// C?p nh?t tín hi?u LATCH
    timer2_counter++;  		// Tang giá tr? d?m c?a Timer2
    // Reset giá tr? d?m khi d?t d?n 8
    if(timer2_counter == 8) timer2_counter = 0;  
    CLOCK = 0;  				// T?t tín hi?u CLOCK
    LATCH = 0;  				// T?t tín hi?u LATCH
}

void Matrix_StartDisplay(void)
{
    T2MOD = 0;       			// Ð?t l?i các thanh ghi di?u khi?n c?a Timer2
    T2CON = 0;       			// Ð?t l?i các thanh ghi c?u hình Timer2
    TL2 = 0xCD;      			// Ð?t giá tr? th?p c?a Timer2
    TH2 = 0xF8;      			// Ð?t giá tr? cao c?a Timer2
    RCAP2L = 0xCD;   			// Ð?t giá tr? n?p l?i th?p c?a Timer2
    RCAP2H = 0xF8;   			// Ð?t giá tr? n?p l?i cao c?a Timer2
    TR2 = 1;         			// B?t Timer2
    ET2 = 1;         			// B?t ng?t Timer2
}
