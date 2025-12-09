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

    void OperatingSystemLoop(void) {
        switch (stateRobot) {
            case STATE_ATTENTE:
                timestamp = 0;
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_ATTENTE_EN_COURS;
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
    unsigned char nextStateRobot = 0;

    void SetNextRobotStateInAutomaticMode() {
        unsigned char positionObstacle = PAS_D_OBSTACLE;
        //ÈDtermination de la position des obstacles en fonction des ÈÈËtlmtres
        if (robotState.distanceTelemetreDroit < 25 &&
                robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreGauche > 30 && 
                robotState.distanceTelemetreExtremeDroite <15 &&
                robotState.distanceTelemetreExtremeGauche>30) //Obstacle ‡droite
            positionObstacle = OBSTACLE_A_DROITE;
        else if (robotState.distanceTelemetreDroit > 30 &&
                robotState.distanceTelemetreCentre > 25 &&
                robotState.distanceTelemetreGauche < 25 &&
                robotState.distanceTelemetreExtremeDroite >30 &&
                robotState.distanceTelemetreExtremeGauche <15) //Obstacle ‡gauche
            positionObstacle = OBSTACLE_A_GAUCHE;
        else if (robotState.distanceTelemetreCentre < 20) //Obstacle en face
            positionObstacle = OBSTACLE_EN_FACE;
        else if (robotState.distanceTelemetreDroit > 30 &&
                robotState.distanceTelemetreCentre > 30 &&
                robotState.distanceTelemetreGauche > 30 &&
                robotState.distanceTelemetreExtremeGauche >30 &&
                robotState.distanceTelemetreExtremeDroite >30 ) //pas d?obstacle
            positionObstacle = PAS_D_OBSTACLE; 
        else if (robotState.distanceTelemetreDroit > 30 &&
                robotState.distanceTelemetreCentre > 30 &&
                robotState.distanceTelemetreGauche < 25 &&
                robotState.distanceTelemetreExtremeGauche <20 &&
                robotState.distanceTelemetreExtremeDroite >30 ) //pas d?obstacle
            positionObstacle = OBSTACLE_A_LEXTREME_GAUCHE;
        
         else if (robotState.distanceTelemetreDroit <25 &&
                robotState.distanceTelemetreCentre > 30 &&
                robotState.distanceTelemetreGauche > 30 &&
                robotState.distanceTelemetreExtremeGauche >30 &&
                robotState.distanceTelemetreExtremeDroite <20 ) //pas d?obstacle
            positionObstacle = OBSTACLE_A_LEXTREME_DROITE;
        
        //ÈDtermination de lÈ?tat ‡venir du robot
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
   
        //Si l?on n?est pas dans la transition de lÈ?tape en cours
        if (nextStateRobot != stateRobot - 1)
            stateRobot = nextStateRobot;
    }

int main(void) {
    //Initialisation oscillateur
    InitOscillator();
    // Configuration des input et output (IO)
    InitIO();
    InitTimer1();
    InitTimer23();
    InitPWM();
    InitADC1();
    InitTimer4();
    //PWMSetSpeed(20,0);
    //PWMSetSpeed(-20,1);// On constate que plus on augmente la vitesse plus la frÈquence augmente
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

    



    // Boucle Principale
    while (1) {


        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();

            unsigned int * result = ADCGetResult();

            float volts = ((float) result [0])* 3.3 / 4096;

            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeDroite = 34 / volts - 5;

            if (robotState.distanceTelemetreDroit < 30) {
                LED_ROUGE_1 = 1;
            } else {
                LED_ROUGE_1 = 0;
            }
            if (robotState.distanceTelemetreCentre < 30) {
                LED_ORANGE_1 = 1;
            } else {
                LED_ORANGE_1 = 0;
            }
            if (robotState.distanceTelemetreGauche < 30) {
                LED_BLEUE_1 = 1;
            } else {
                LED_BLEUE_1 = 0;
            }
            if (robotState.distanceTelemetreExtremeGauche< 30) {
                LED_VERTE_1 = 1;
            } else {
                LED_VERTE_1 = 0;
            }       
            if (robotState.distanceTelemetreExtremeDroite < 30) {
                LED_BLANCHE_1 = 1;
            } else {
                LED_BLANCHE_1 = 0;
            }
        }


    } // fin main

}