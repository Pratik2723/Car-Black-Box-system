#include <xc.h>
#include "black_box.h"
#include "CLCD.h"
#include "matrix_key.h"
#include "EEPROM.h"
#include "UART.h"
#include "DS1307.h"

extern State_t state;
unsigned char view_event[10][15], addr = 0;
unsigned char l, j, k = 0;
extern unsigned char event_count, key, i, flag, menu, time[9];

void event_reader(void)
{
    for(l = 0; l < event_count; l++)
    {
        for(j = 0; j < 14; j++)
        {
            if(j == 2 || j == 5)
            {
                view_event[l][j] = ':';
            }
            else if(j == 8 || j == 11)
            {
                view_event[l][j] = ' ';
            }
            else
            {
                view_event[l][j] = read_external_eeprom(addr++);
            }
        }
        if(addr == 100)
        {
            addr = 0;
        }
        view_event[l][j] = '\0';
    }
    addr = 0;
}

void view_log(void) 
{
    event_reader();
    if(event_count == 0)
    {
        clcd_print("Event Log", LINE1(0));
        clcd_print("is Empty!", LINE2(7));
        __delay_ms(1000);
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
    }
    else
    {
        clcd_print("TIME     EV SP", LINE1(2));
        
        if (key == MK_SW2) 
        {              
            if (k != 9 && k < event_count - 1) 
            {
                CLEAR_DISP_SCREEN;
                k++;
            }
        } 
        else if (key == MK_SW1) 
        {
            if (k != 0) 
            {            
                k--;
            }
        }
        
        if(k < event_count)
        {
            clcd_putch(k + '0', LINE2(0));
            clcd_putch(' ', LINE2(1));
            clcd_print(view_event[k], LINE2(2));
        }
        
        if (key == MK_SW12) 
        {
            CLEAR_DISP_SCREEN;
            i = 0;
            flag = 0;
            l = 0;
            j = 0;
            k = 0;
            state = e_main_menu;
        }
    }  
}

void clear_log(void) 
{
    event_count = 0;
    
    clcd_print("Log Cleared", LINE1(0));
    clcd_print("Successfully", LINE2(3));
    __delay_ms(1000);
    menu = 0;
    CLEAR_DISP_SCREEN;
    i = 0;
    flag = 0;
    state = e_main_menu;
}

void download_log(void)
{
    unsigned int iter;
    event_reader();
    if(event_count != 0)
    {
        for(iter = 0; iter < event_count; iter++)
        {
            puts(view_event[iter]);
            puts("\n\r");
        }
        iter = 0;
        clcd_print("Log Downloaded", LINE1(0));
        clcd_print("Successfully", LINE2(3));
    }
    else
    {
        clcd_print("Log is Empty!!", LINE1(0));
    }
    
    __delay_ms(800);
    menu = 0;
    CLEAR_DISP_SCREEN;
    i = 0;
    flag = 0;
    state = e_main_menu;
}

void set_time(void)
{
    static unsigned int field = 0;
    static unsigned int set_delay = 0;
    static unsigned int set_flag = 0;
    static unsigned int hr = 0, min = 0 , sec = 0;
    clcd_print("HH MM SS", LINE1(0));
    //key = read_matrix_keypad(STATE_CHANGE);
    
    if(set_flag == 0)
    {
        hr = ((time[0] - 48) * 10) + (time[1] - 48);
        min = ((time[3] - 48) * 10) + (time[4] - 48);
        sec = ((time[6] - 48) * 10) + (time[7] - 48);  
        set_flag = 1;
    }
    
    if(key == MK_SW2)
    {
        field = (field + 1) % 3;
    }
    if(key == MK_SW1)
    {
        if(field == 0)
        {
            if(hr < 23)
                hr++;
            else
                hr = 0;
        }
        else if(field == 1)
        {
            if(min < 59)
                min++;
            else
                min = 0;
        }
        else if(field == 2)
        {
            if(sec < 59)
                sec++;
            else
                sec = 0 ;
        }
    }
        
    if(set_delay++ < 600)
    {
        clcd_putch((hr / 10) + '0',LINE2(0));
        clcd_putch((hr % 10) + '0',LINE2(1)); 
        clcd_putch(':',LINE2(2));
        clcd_putch((min / 10) + '0',LINE2(3));
        clcd_putch((min % 10) + '0',LINE2(4));
        clcd_putch(':',LINE2(5));
        clcd_putch((sec / 10) + '0',LINE2(6));
        clcd_putch((sec % 10) + '0',LINE2(7)); 
    }
    else if(set_delay++ < 1000)
    {
        if(field == 0)
        {
            clcd_putch(' ',LINE2(0));
            clcd_putch(' ',LINE2(1));
        }
        else if(field == 1)
        {
            clcd_putch(' ',LINE2(3));
            clcd_putch(' ',LINE2(4));
        }
        else if(field == 2)
        {
            clcd_putch(' ',LINE2(6));
            clcd_putch(' ',LINE2(7));
        }
    }
    else
    {
        set_delay = 0;
    }
    
    if(key == MK_SW11)
    {
        write_ds1307(0x02, (hr / 10) << 4 | (hr % 10));
        write_ds1307(0x01, (min / 10) << 4 | (min % 10));
        write_ds1307(0x00, (sec / 10) << 4 | (sec % 10));
        set_flag = 0;
        CLEAR_DISP_SCREEN;
        clcd_print("TIME SAVED", LINE1(3));
        clcd_print("exiting...", LINE2(0)); 
        menu = 0;
        flag = 0;
        i = 0;
        __delay_ms(800);
        CLEAR_DISP_SCREEN;
        state = e_dashboard;
    }
    else if(key == MK_SW12)
    {
        CLEAR_DISP_SCREEN;
        clcd_print("TIME NOT SAVED", LINE1(0));
        clcd_print("exiting...", LINE2(0));
        menu = 0;
        flag = 0;
        i = 0;
        __delay_ms(800);
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
    }
}