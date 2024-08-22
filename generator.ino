/* Генератор 1 Hz..8 MHz. Энкодер подключен к пинам A0 и A1, кнопка 
энкодера подключена к A2. Требуется использовать конденсаторы  0,01..0,1uf 
относительно земли на каждый из 2х выводов энкодера.
Скетч для ардуино на мк atmega328 (UNO,Nano, MiniPro)
*/

float freq; 
void setup() {
pinMode (9,OUTPUT); // выход генератора
pinMode(A0,INPUT_PULLUP); // с рассчетом, что  энкодере внешняя подтяжка-   
pinMode(A1,INPUT_PULLUP); // -к шине питания. Если нету, то подтянуть программно. 
pinMode(A2,INPUT_PULLUP); //кнопка энкодера 

Serial.begin(9600);
PCICR=1<<PCIE1; //разрешить прерывание PCINT
PCMSK1=(1<<PCINT9);// По сигналу на А1 создавать прерывание
TCCR1A=1<<COM1A0; //подключить выход OC1A первого таймера
TCCR1B=0;//
}


ISR (PCINT1_vect){
static boolean gen_mode=0; //флаг режима управления
uint32_t ocr=OCR1A;
uint32_t divider=1; //переменная коэфф. деления прескалера

static uint32_t enc = 400; //переменная счёта энкодера
uint32_t min_freq = 300;
uint32_t max_freq = 600;
uint32_t step = 1;


byte n=PINC&3; //считать значение энкодера
boolean knopka = PINC&(1<<2); // 0-кнопка нажата, 1-кнопка НЕ нажата.
if (freq<2848) gen_mode=0; //переключение режима управления по частоте
if (freq>=2848) gen_mode=1; //переключение режима управления по OCR

// Если  увеличение частоты
if (n==3||n==0){ 
 if (enc < max_freq){
  enc += step;
 }
 } //end GetUP

// Если уменьшение частоты
if (n==2||n==1){ 
 if (enc > min_freq){
  enc -= step;
  }
 }


if(gen_mode){ OCR1A=ocr;  freq= (float)F_CPU/2 / (OCR1A+1);  } 
else { //расчёт прескалера и OCR по нужной частоте
divider=1; ocr = (F_CPU / enc /2 /divider);
if (ocr >65536) { divider=8; ocr = F_CPU / enc /2 /divider;
    if (ocr >65536) { divider=64; ocr = F_CPU / enc /2 /divider;
        if (ocr >65536)  {divider=256; ocr = F_CPU / enc /2 /divider;
            if (ocr >65536) { divider=1024; ocr = F_CPU / enc /2 /divider;
                if (ocr >65536){ocr=65536; }}}}} OCR1A=ocr-1; 
//запись в регистр прескалера            
switch (divider) {
  case 1: TCCR1B=1|(1<<WGM12); break;
   case 8: TCCR1B=2|(1<<WGM12); break;
    case 64: TCCR1B=3|(1<<WGM12); break;
     case 256: TCCR1B=4|(1<<WGM12); break;
      case 1024: TCCR1B=5|(1<<WGM12); break;  }

    freq= (float) F_CPU/2 / (OCR1A+1) /divider;
  } //end if !gen_mode
}


void loop() {

  
if (freq <10000) { Serial.print(freq,1);Serial.println(" Hz "); }
if (freq >10000) { Serial.print(freq/1000,3);Serial.println(" kHz");}


delay(100);
}
