///////////////////////////////////////////////////////////////////////////
////                             MLCD.C                                ////
////               Driver for LCD Module on CCS PICENS                 ////
////                                                                   ////
////  lcd_init()   Must be called before any other function.           ////
////                                                                   ////
////  lcd_putc(c)  Will display c on the next position of the LCD.     ////
////                     The following have special meaning:           ////
////                      \f  Clear display                            ////
////                      \n  Go to start of second line               ////
////                      \b  Move back one position                   ////
////                                                                   ////
////  lcd_gotoxy(x,y) Set write position on LCD (upper left is 1,1)    ////
////                                                                   ////
////  lcd_getc(x,y)   Returns character at position x,y on LCD         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// RELEASE HISTORY:                                                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

// As defined in the following structure the pin connection is as follows:
//     E2  enable
//     E0  rs
//     E1  rw
//     D4  D4
//     D5  D5
//     D6  D6
//     D7  D7
//
//   LCD pins D0-D3 are not used


struct lcd_pin_map {                 // This structure is overlayed
           int     empty: 4;         // on PORT D
           int     data : 4;
        } lcd_io,lcd_lat,lcd_tris;

#bit lcd_enable=0xF8D.2
#bit lcd_rs=0xF8D.0
#bit lcd_rw=0xF8D.1

#byte lcd_io = 0xF83                   // This puts the entire structure on PORT D
#byte lcd_lat = 0xF8C
#byte lcd_tris = 0xF95

#define lcd_type 2           // 0=5x7, 1=5x10, 2=2 lines
#define lcd_line_two 0x40    // LCD RAM address for the second line


BYTE const LCD_INIT_STRING[4] = {0x20 | (lcd_type << 2), 0xc, 1, 6};
                             // These bytes need to be sent to the LCD
                             // to start it up.


                             // The following are used for setting
                             // the I/O port direction register.


BYTE lcd_read_byte() {
      BYTE low,high;
      lcd_tris.data=0xF;  //set_tris_lcd(LCD_READ);
      lcd_rw = 1;
      delay_us(1);
      lcd_enable = 1;
      delay_us(1);
      high = lcd_io.data;
      lcd_enable = 0;
      delay_us(1);
      lcd_enable = 1;
      delay_us(1);
      low = lcd_io.data;
      lcd_enable = 0;
      lcd_tris.data=0;  //set_tris_lcd(LCD_WRITE);
      return( (high<<4) | low);
}


void lcd_send_nibble( BYTE n ) {
      lcd_lat.data = n;
      delay_us(1);
      lcd_enable = 1;
      delay_us(2);
      lcd_enable = 0;
}


void lcd_send_byte( BYTE address, BYTE n ) {

      lcd_rs = 0;
      delay_us(1);
      while ( bit_test(lcd_read_byte(),7) ) ;
      lcd_rs = address;
      delay_us(1);
      lcd_rw = 0;
      delay_us(1);
      lcd_enable = 0;
      lcd_send_nibble(n >> 4);
      lcd_send_nibble(n & 0xf);
}


void lcd_init() {
    BYTE i;
    lcd_tris.data=0;    //set_tris_lcd(LCD_WRITE);
    lcd_rs = 0;
    lcd_rw = 0;
    lcd_enable = 0;
    *0xF96=*0xF96 & 0xF8;
    delay_ms(15);
    for(i=1;i<=3;++i) {
       lcd_send_nibble(3);
       delay_ms(5);
    }
    lcd_send_nibble(2);
    delay_ms(5);
    for(i=0;i<=3;++i)
       lcd_send_byte(0,LCD_INIT_STRING[i]);
}


void lcd_gotoxy( BYTE x, BYTE y) {
   BYTE address;

   if(y!=1)
     address=lcd_line_two;
   else
     address=0;
   address+=x-1;
   lcd_send_byte(0,0x80|address);
}

void lcd_putc( char c) {
   switch (c) {
     case '\f'   : lcd_send_byte(0,1);
                   delay_ms(2);
                                           break;
     case '\n'   : lcd_gotoxy(1,2);        break;
     case '\b'   : lcd_send_byte(0,0x10);  break;
     default     : lcd_send_byte(1,c);     break;
   }
}

char lcd_getc( BYTE x, BYTE y) {
   char value;

    lcd_gotoxy(x,y);
    while ( bit_test(lcd_read_byte(),7) ); // wait until busy flag is low
    lcd_rs=1;
    value = lcd_read_byte();
    lcd_rs=0;
    return(value);
}
