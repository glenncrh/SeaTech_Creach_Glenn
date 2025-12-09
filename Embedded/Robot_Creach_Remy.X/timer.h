#ifndef TIMER_H
#define TIMER_H
void InitTimer23(void);
void InitTimer1(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);
void SetFreqTimer1(float);
void InitTimer4(void);
void SetFreqTimer4(float);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void);
extern unsigned long timestamp;

#endif /* TIMER_H */
