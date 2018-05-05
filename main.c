#include <REG51.H>

xdata unsigned char PTWY _at_ 0x8008;
xdata unsigned char PSEG _at_ 0x8018;

unsigned  char xdata cyferki[16] = {  0x3f,	0x06, 0x5b,	0x4f,	0x66,	0x6d,	0x7d,	0x07,	0x7f,	0x6f,	0x77, 0x79, 0x0F, 0x1F,	0x0E, 0X71};

unsigned char seg=0, segmask=0, PTSEGB, kol_val = 0, wiersz_val = 0, key_val =0;

int Pr = 0;

// PWM_Pin
sbit PWM_Pin = 0x96;

// Deklaracje
void Init(void);

// Zmienne PWM
unsigned char PWM = 0;	  //  0 (0%) - 255 (100%)
unsigned int temp = 0;    // Dla przerwania

int PWMfreq = 1;	// Czestotliwosc

// LCD
xdata unsigned char CTRLW _at_ 0x8010;
xdata unsigned char DATW _at_ 0x8011;
xdata unsigned char CTRLR _at_ 0x8012;
xdata unsigned char DATR _at_ 0x8013;


// ADC
xdata unsigned char PTAC0 _at_ 0x8005;
unsigned char POT0(void);


unsigned int cycles = 0;
unsigned int cyfra = 0;
 unsigned int b;
 int h = 0;
void BUSYLCD(void)
{
 while (CTRLR&0x80);
}

void main (void)
{

  Init();              // Inicjalizacja         
	


	// LCD INIT	
	

 for(b=0;b<15000;b++);
 CTRLW=0x38;
 for(b=0;b<4200;b++);
 CTRLW=0x38;
 for(b=0;b<120;b++);
 CTRLW=0x38;
 BUSYLCD();
 CTRLW=0x38;
 BUSYLCD();
 CTRLW=0x0c;
 BUSYLCD();
 CTRLW=0x02;
 CTRLW=0x01;
 BUSYLCD();
 CTRLW=0x06;
 BUSYLCD();
 CTRLW=0x80;
 BUSYLCD();


 
 while(1)
 {
	PWMfreq = 250;
	
  b=POT0();
  PWM = b; 
  DATW = 'P';
  BUSYLCD();
  DATW = 'O';
  BUSYLCD();
  DATW = 'T';
  BUSYLCD();
  DATW = '0';
  BUSYLCD();
  DATW = ' ';
  BUSYLCD();
  DATW=Pr/100+'0';
  BUSYLCD();
  b=Pr%100;
  DATW=Pr/10+'0';
  BUSYLCD();
  DATW=Pr%10+'0';
  BUSYLCD();
  CTRLW=0x80; 
  BUSYLCD();
	
	
  DATW = 'P';
  BUSYLCD();
  DATW = 'W';
  BUSYLCD();
  DATW = 'M';
  BUSYLCD();
  DATW = ' ';
  BUSYLCD();
  DATW = 'f';
	BUSYLCD();
  DATW = ' ';
  BUSYLCD();
  DATW=PWMfreq/100+'0';
  BUSYLCD();
  PWMfreq=PWMfreq%100;
  DATW=PWMfreq/10+'0';
  BUSYLCD();
  DATW=PWMfreq%10+'0';
  BUSYLCD();
  CTRLW=0xC0; 
  BUSYLCD();
  
  
 
  
 }
}


// Timer0 init
void InitTimers(void)
{
TMOD = 0x11;    // Zalaczanie przerwania 1 i 2
TCON = 0x50;
TH0 = 0x00;      // First time value
TL0 = 0x00; 
TH1 = 0x00;      // First time value
TL1 = 0x00;	
ET0 = 1;         // Enable przerwan
EA  = 2;        
TR0 = 1; 
TR1 = 1;	
	ET1 = 1;
}


// Odczyt z ADCka POT0
unsigned char POT0(void)
{
 unsigned char a=0;
 PTAC0=0;
 for(a=0;a<120;a++);
 return PTAC0;
}



// Init PWM & Timer�w
void Init(void)
{
PWM = 0;         // Wypelnienie 0
InitTimers();    // Uruchom timer przerwan

}

// Timer0
void Timer0 (void) interrupt 1  
{
TR0 = 0;    // Stop Timer 0
 
TH0 = (unsigned char)(-5000 >> 8);
TL0 = (unsigned char)(-5000 & 0x00ff);
	
if(PWM_Pin)	// Na stan wysoki
{
PWM_Pin = 0;
temp = (255-PWM)*PWMfreq;
TH0  = 0xFF - (temp>>8)&0xFF;
TL0  = 0xFF - temp&0xFF;
}
else	     // Na stan niski
{
PWM_Pin = 1;
temp = PWM*PWMfreq;
TH0  = 0xFF - (temp>>8)&0xFF;
TL0  = 0xFF - temp&0xFF;
}
 
TF0 = 0;     // Czyszczenie flagi przerwania
TR0 = 1;     // Start Timera
}


void Timer1 (void) interrupt 3  
{
TR1 = 0;    // Stop Timer 0
 
TH1 = (unsigned char)(-5000 >> 8);
TL1 = (unsigned char)(-5000 & 0x00ff);
	
			seg=++seg&3; 
	segmask=1<<seg;
		P1=(P1&0xc0)|0x3c|seg;
	  PSEG=cyferki[key_val];

	for(wiersz_val = 0;wiersz_val<4; wiersz_val++)
		{
				if(!(P1&(4<<wiersz_val)))
			{
				seg = seg + wiersz_val*4;
				PTWY = P1;
				Pr = P1;
			}
		}
	
 
TF1 = 0;     // Czyszczenie flagi przerwania
TR1 = 1;     // Start Timera
}
