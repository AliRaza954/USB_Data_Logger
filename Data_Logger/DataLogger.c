#include <18F452.h>
#device ADC=10
#include "kbd2.c" 
#use delay(clock=10000000)
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#include "mlcd.c"
#include "ds1302.c"
#include <string.h>
byte s[12];
int a[10];
int i=0;
int c;
long int q;
float r,r1;  
#INT_RDA
void serial_isr()
{
  
  c =getc();
  a[i++]=c;
 }

byte get_number() {
  char first,second;

  do {
    first=kbd_getc();
  } while ((first<'0') || (first>'9'));
  lcd_putc(first);
  first-='0';
    delay_ms(100);
  do {
    second=kbd_getc();
  } while ((second<'0') || (second>'9'));
  lcd_putc(second);
  second-='0';
  delay_ms(100);
  return((first*10)+second);
}

void set_clock(){
   byte day,mth,year,dow,hour,min;

   lcd_putc("\fYear 20: ");
   year=get_number();
   delay_ms(50);
   lcd_putc("\fMonth: ");
   mth=get_number();
    delay_ms(50);
   lcd_putc("\fDay: ");
   day=get_number();
    delay_ms(50);
   lcd_putc("\fWeekday 1-7: ");
   dow=get_number();
    delay_ms(50);
   lcd_putc("\fHour: ");
   hour=get_number();
   delay_ms(50);
   lcd_putc("\fMin: ");
   min=get_number();

   rtc_set_datetime(day,mth,year,dow,hour,min);
}

void set_clock1(){
      
   
  lcd_putc("\fST-HOUR: ");
   s[6]=get_number();
    delay_ms(50);
   lcd_putc("\fST-MIN: ");
   s[7]=get_number();
    delay_ms(50);
   lcd_putc("\fEND-HOUR: ");
   s[8]=get_number();
    delay_ms(50);
   lcd_putc("\fEND-MIN: ");
  s[9]=get_number();
   delay_ms(50);
   lcd_putc("\fDUR-HOUR: ");
   s[10]=get_number();
   lcd_putc("\fDUR-MIN: ");
   s[11]=get_number();
 delay_ms(50);
  
}

void main()
{
char cmd;
char Got_Big_E = 0x00;
char Got_Small_e = 0x00;
byte day,mth,year,dow,hour,min,sec,x,y,p;
port_b_pullups(true);
set_tris_c(0x88);
//set_tris_c(0x98);
set_tris_D(0x00);
output_low(PIN_C2);//SET CTS LOW
setup_port_a( ALL_ANALOG );
setup_adc( ADC_CLOCK_INTERNAL );

enable_interrupts(INT_RDA);
enable_interrupts(GLOBAL);
i=0;
lcd_init();
rtc_init();
kbd_init();
s:
lcd_putc("\f");  
printf(lcd_putc,"STARTING");
delay_ms(500);
i=0;
while (Got_Big_E == 0x00){
while(input_state(PIN_C3)); //wait for RTS low
putc('E'); 
while(input_state(PIN_C3));                          // Send 'e'
putc(0x0D);  
delay_ms(10); 

if ((a[0] == 'E')&&(a[1] == 0x0D))
  {
   Got_Big_E = 0x01;
   lcd_putc("\f");  
   printf(lcd_putc,"GOT BIG E");
  }
 else
  {
   delay_ms(10);                        // Wait a bit and retry synchronisation
  }
}
i=0;
while (Got_small_e == 0x00){
while(input_state(PIN_C3));
putc('e');                           // Send 'e'
while(input_state(PIN_C3));
putc(0x0D);
delay_ms(10);   
a[i++]=c;
if ((a[0] == 'e') && (a[1] == 0x0D))
  {
   Got_small_e = 0x01;
   lcd_putc("\f");  
   printf(lcd_putc,"GOT SMALL e");
  }
 else
  {
   delay_ms(10);   // Wait a bit and retry synchronisation
  }
}
lcd_putc("\f");  
printf(lcd_putc,"SYNC SUCCESS");
delay_ms(500);  
i=0;
putc(0x0D); 
delay_ms(10);
a[i++]=c;
if ((a[0] == 'D') && (a[1] == ':') && (a[2]== 0x5C) && (a[3] == '>') && (a[4] == 0x0D)) // Check for prompt
   {
   lcd_putc("\f");   
   printf(lcd_putc,"DISK FOUND");   // If prompt found, then return disk available
   }
   else
   {
   lcd_putc("\f");  
   printf(lcd_putc,"DISK NOT FOUND");
   delay_ms(500);
    goto s;                             // Return no disk available
   }


//lcd_putc("\f1: Change, 2: Display");
lcd_putc("\f PRESS 1 TO SET ");
   do {
      cmd=kbd_getc();
   } while ((cmd!='1')&&(cmd!='2'));

if(cmd=='1')
     set_clock();
   s1:
 
     set_clock1();
   while (1) {
     lcd_putc('\f');
      rtc_get_date( day, mth, year, dow);
      rtc_get_time( hour, min, sec );
      printf(lcd_putc,"%2u/%2u/%2u\n%2u:%2u:%2u",day,mth,year,hour,min,sec);
      delay_ms(250);
      s[4]=hour;
      s[5]=min;
      if((s[6]==s[4]) && (s[7]==s[5])){
      output_high(pin_d0);
      while(1){
      lcd_putc('\f');
      rtc_get_date( s[0],s[1],s[2],s[3]);
      rtc_get_time( s[4],s[5], sec );
      x= s[6]+s[10];
      y=s[7]+s[11];
      if(x>=24)
        x=x%24;
      if(y>=60){
        p=y/60;
        y=y%60;
        x=s[6]+p;
       if(x>=24)
        x=x%24;}
   
      printf(lcd_putc,"%2u:%2u:%2u\n%2u:%2u",s[4],s[5],sec,x,y);
      delay_ms(250);     
      if((x==s[4]) && (y==s[5])){     
      s[6]= x;
      s[7]=y;
 i=0;
 while(i<1){
 output_high(pin_d0);
 delay_ms(500);
 output_low(pin_d0);
 delay_ms(500);
 set_adc_channel(0);
 delay_ms(100);
 q = read_adc();
 r = 5.0 * q / 1023.0;
 delay_ms(10);
 set_adc_channel(1);
 delay_ms(100);
 q = read_adc();
 r1 = 500 * q / 1023.0;  
 delay_ms(10);  
 printf("OPW Results.dat");
 putc(0x0D);
 putc('I');
 putc('P');
 putc('A');
 putc(0x0D); 
 putc('W');
 putc('R');
 putc('F');
 putc(' ');
 putc(0x00);
 putc(0x00);
 putc(0x00);
 putc(0x1B);
 putc(0x0D);  
 printf("%f\t%f\n",r,r1 );
 putc(0x0D); 
 printf("CLF Results.dat");
 //putc(0x0A);
 putc(0x0D);
 
 i++;
 }
 if((s[8]==s[4]) && (s[9]==s[5])){ 
        output_low(pin_d0);      
         lcd_putc('\f');
         printf(lcd_putc,"MEASUREMENT COMPLETED\n");        
         
         delay_ms(100);
         goto s1;
        }
 

}//if  

 
        

        }//while


  

          
     
   }//if 


   }
 
}



