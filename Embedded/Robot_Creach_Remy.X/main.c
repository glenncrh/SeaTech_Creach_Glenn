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
#include "Robot.h"
#include "adc.h"
#include "main.h"

unsigned char stateRobot;
unsigned char nextStateRobot = 0;

/* ===================== MACHINE A ETATS ===================== */

void OperatingSystemLoop(void) {
    switch (stateRobot) {

        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;   // ? correction

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

/* ===================== DETECTION OBSTACLES ===================== */

void SetNextRobotStateInAutomaticMode() {

    unsigned char positionObstacle = PAS_D_OBSTACLE;

    // Détection simple : 1 capteur = 1 obstacle
    if (robotState.distanceTelemetreCentre < 30)
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreGauche < 30)
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreExtremeGauche < 30)
        positionObstacle = OBSTACLE_A_LEXTREME_GAUCHE;
    else if (robotState.distanceTelemetreDroit < 30)
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreExtremeDroite < 30)
        positionObstacle = OBSTACLE_A_LEXTREME_DROITE;

    // Choix de l'état suivant
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_LEXTREME_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_A_LEXTREME_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;

    // Sécurité transition
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
}

/* ===================== MAIN ===================== */

int main(void) {

    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer23();
    InitPWM();
    InitADC1();
    InitTimer4();

    LED_BLANCHE_1 = 0;
    LED_BLEUE_1 = 0;
    LED_ORANGE_1 = 0;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    LED_VERTE_2 = 0;
    LED_ROUGE_2 = 0;
    LED_ORANGE_2 = 0;
    LED_BLEUE_2 = 0;
    LED_BLANCHE_2 = 0;
    
    PWM_ENABLE = 1;

    stateRobot = STATE_ATTENTE;

    while (1) {
        PWMUpdateSpeed();
        if (ADCIsConversionFinished() == 1) {

            ADCClearConversionFinishedFlag();
            unsigned int *result = ADCGetResult();

            float volts;

            volts = ((float)result[0]) * 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;

            volts = ((float)result[1]) * 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;

            volts = ((float)result[2]) * 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;

            volts = ((float)result[3]) * 3.3 / 4096;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;

            volts = ((float)result[4]) * 3.3 / 4096;
            robotState.distanceTelemetreExtremeDroite = 34 / volts - 5;

            // LEDs = détection brute
            LED_ROUGE_1   = (robotState.distanceTelemetreDroit < 30);
            LED_ORANGE_1  = (robotState.distanceTelemetreCentre < 30);
            LED_BLEUE_1   = (robotState.distanceTelemetreGauche < 30);
            LED_VERTE_1   = (robotState.distanceTelemetreExtremeGauche < 30);
            LED_BLANCHE_1 = (robotState.distanceTelemetreExtremeDroite < 30);
        }

        
    }
}
