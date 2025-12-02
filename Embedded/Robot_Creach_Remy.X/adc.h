
#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
void InitADC1(void);
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);
unsigned int * ADCGetResult(void);

#endif	/* ADC_H */

