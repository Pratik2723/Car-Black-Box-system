/*
 * Name: Pratik Jadhav
 * Created on 28 February, 2025
 */

#include <xc.h>
#include "black_box.h"
#include "CLCD.h"
#include "matrix_key.h"
#include "I2C.h"
#include "DS1307.h"
#include "EEPROM.h"
#include "UART.h"

State_t state;
unsigned char event[9][3] = {"ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR", "C_"}, speed, key;
unsigned char time[9], clock_reg[3];
unsigned char gear = 0, event_count = 0;

void init_config()
{
    init_adc();
    init_clcd();
    init_matrix_keypad();
    init_adc();
    init_i2c();
    init_ds1307();
    init_uart();
    
    state = e_dashboard;
}

static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	
	time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
	time[1] = '0' + (clock_reg[0] & 0x0F);
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x07);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x07);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}

void main(void) 
{
    init_config();

    while(1)
    {
        key = read_switches(STATE_CHANGE);
        // Detect key press

        switch (state)
        {
            case e_dashboard:
                // Display dashboard
                view_dashboard();
                break;
            
            case e_main_menu:
                // Display dashboard
                display_main_menu();
                break;
            
            case e_view_log:
                // Display dashboard
                view_log();
                break;
                 
            case e_download_log:
                download_log();
                break;
                
            case e_clear_log:
                clear_log();
                break;
                      
            case e_set_time:
                set_time();
                break;        
        }
        
    }
    
}

void view_dashboard(void)
{
    get_time();
    clcd_print(time, LINE2(0));
    clcd_print("TIME     EV  SP ", LINE1(0));
    clcd_print(event[gear], LINE2(9));
    

    if(key == MK_SW3)
    {
        gear = 8;
        clcd_print(event[gear], LINE2(9));
        event_count++;
        event_store();
    }

    else if(key == MK_SW1)
    {
        if(gear >= 0 && gear < 7)
        {
            gear++;
            clcd_print(event[gear], LINE2(9));                
        }
        event_count++;
        event_store();
    }

    else if(key == MK_SW2)
    { 
        if(gear <= 7 && gear > 1)
        {
            gear--;
            clcd_print(event[gear], LINE2(9));                
        }
        event_count++;
        event_store();
    }

    if(gear == 8)
    {
        if(key == MK_SW1 || key == MK_SW2)
        {
            gear = 1;
            event_count++;
            event_store();
        }
    }
    
    if(key == MK_SW11)
    {
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
    }

    speed = read_adc() / 10.33;
    clcd_putch((speed / 10) + 48, LINE2(13));
    clcd_putch((speed % 10) + 48, LINE2(14));
}


void event_store(void)
{
    static unsigned char addr = 0;
    if(addr == 100)
    {
        addr = 0;
    }
    
    // run the loop for 8 times to store time
    for(unsigned int i = 0; i < 8; i++)
    {
        if (i == 2 || i == 5) {
            continue;
        }
        write_external_eeprom(addr, time[i]);
        addr++;
    }
    
    // run the loop for 2 times to store event
    for(unsigned int i = 0; i < 2; i++)
    {
        write_external_eeprom(addr, event[gear][i]);
        addr++;
    }
    
    //store the speed
    write_external_eeprom(addr++, (speed / 10 + 48));
    write_external_eeprom(addr++, (speed % 10 + 48));
}