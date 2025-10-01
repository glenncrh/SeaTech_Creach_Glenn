/* 
 * File:   main.c
 * Author: E306-PC6
 *
 * Created on September 29, 2025, 3:19 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"

int main(void) {
    //Initialisation oscillateur
    InitOscillator();
    // Configuration des input et output (IO)
    InitIO();
    InitTimer1();
    InitTimer23();
    InitPWM();
    //PWMSetSpeed(20,0);
    //PWMSetSpeed(-20,1);// On constate que plus on augmente la vitesse plus la fr�quence augmente
  
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;
    LED_VERTE_2 =1;
    LED_ROUGE_2=1;
    LED_ORANGE_2=1;
    LED_BLEUE_2=1;
    LED_BLANCHE_2 = 1;

    // Boucle Principale
    while (1) {
        
    } // fin main
    
}