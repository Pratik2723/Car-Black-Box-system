#include <xc.h>
#include "black_box.h"
#include "CLCD.h"
#include "matrix_key.h"

extern State_t state;
unsigned char i = 0, flag = 0, menu = 0;
unsigned char main_menu[4][16] = {"View Log       ", "Download Log   ", "Clear Log      ", "Set Time      "};
extern unsigned char key;

void display_main_menu(void) 
{
    if (i < 3) {
        clcd_print(main_menu[i], LINE1(3));
        clcd_print(main_menu[i + 1], LINE2(3));
    } 
    else {
        clcd_print(main_menu[i - 1], LINE1(3));
        clcd_print(main_menu[i], LINE2(3));
    }


    if (flag == 0) {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
    } 
    else {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
    }

    if (key == MK_SW2) {
        CLEAR_DISP_SCREEN;
        if (menu < 3) {
            menu++;
        }

        if (i < 3 && flag == 1) {
            i++;
        }
        flag = 1;
    } 
    else if (key == MK_SW1) {
        CLEAR_DISP_SCREEN;
        if (menu > 0) {
            menu--;
        }

        if (i > 0 && flag == 0) {
            i--;
        }
        flag = 0;
    } 
    else if (key == MK_SW11) {
        CLEAR_DISP_SCREEN;
        state = menu + 2;
    } 
    else if (key == MK_SW12) {
        CLEAR_DISP_SCREEN;
        i = 0;
        menu = 0;
        flag = 0;
        state = e_dashboard;
    }
}