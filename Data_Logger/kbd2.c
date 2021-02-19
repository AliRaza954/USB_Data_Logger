///////////////////////////////////////////////////////////////////////////
////                             KBDD.C                                ////
////                  Generic keypad scan driver                       ////
////                                                                   ////
////  kbd_init()   Must be called before any other function.           ////
////                                                                   ////
////  c = kbd_getc(c)  Will return a key value if pressed or /0 if not ////
////                   This function should be called frequently so as ////
////                   not to miss a key press.                        ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////
//Keypad connection: //Jeevan keyboard
#define col0 PIN_B4 
#define col1 PIN_B5 
#define col2 PIN_B6 
#define col3 PIN_B7 
#define row0 PIN_B0 
#define row1 PIN_B1 
#define row2 PIN_B2 
#define row3 PIN_B3 


//Keypad connection: 
//#define row0 PIN_B0 
//#define row1 PIN_B1 
//#define row2 PIN_B2 
//#define row3 PIN_B3 
//#define col0 PIN_B7 
//#define col1 PIN_B6 
//#define col2 PIN_B5 
//#define col3 PIN_B4 
// Keypad layout: 
char const KEYS[4][4] = 
{{'0','1','2','3'}, 
 {'4','5','6','7'}, 
 {'8','9','A','B'}, 
 {'C','D','E','F'}}; 


//#define KBD_DEBOUNCE_FACTOR 33 // Set this number to apx n/333 where 
// n is the number of times you expect 
// to call kbd_getc each second 
#define KBD_DEBOUNCE_FACTOR 150 //
void kbd_init() 
{ 
//set_tris_b(0xF0); 
//output_b(0xF0); 
port_b_pullups(true);  
} 

short int ALL_ROWS (void) 
{ 
if(input (row0) & input (row1) & input (row2) & input (row3)) 
   return (0); 
else 
   return (1); 
} 



char kbd_getc() 
{ 
static byte kbd_call_count; 
static short int kbd_down; 
static char last_key; 
static byte col; 

byte kchar; 
byte row; 

kchar='\0'; 

if(++kbd_call_count>KBD_DEBOUNCE_FACTOR) 
  { 
   switch (col) 
     { 
      case 0: 
        output_low(col0); 
        output_high(col1); 
        output_high(col2); 
        output_high(col3); 
        break; 
    
      case 1: 
        output_high(col0); 
        output_low(col1); 
        output_high(col2); 
        output_high(col3); 
        break; 

      case 2: 
        output_high(col0); 
        output_high(col1); 
        output_low(col2); 
        output_high(col3); 
        break; 

      case 3: 
        output_high(col0); 
        output_high(col1); 
        output_high(col2); 
        output_low(col3); 
        break; 
      } 

   if(kbd_down) 
     { 
      if(!ALL_ROWS()) 
        { 
         kbd_down=false; 
         kchar=last_key; 
         last_key='\0'; 
        } 
     } 
   else 
     { 
      if(ALL_ROWS()) 
        { 
         if(!input (row0)) 
            row=0; 
         else if(!input (row1)) 
            row=1; 
         else if(!input (row2)) 
            row=2; 
         else if(!input (row3)) 
            row=3; 

         last_key =KEYS[row][col]; 
         kbd_down = true; 
        } 
      else 
        { 
         ++col; 
         if(col==4) 
            col=0; 
        } 
     } 
   kbd_call_count=0; 
  } 
return(kchar); 
} 

//}
