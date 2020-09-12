#ifndef INFRARED_H
#define INFRARED_H

#include <hal_types.h>

extern uint8 a;
extern uint8 AC_off[];
extern uint8 AC_on[];


void Air_conditioner_on(int);
void Air_conditioner_off(int);
void Air_conditioner_control(int TP1);
extern unsigned char Air_conditioner_mode(void);
extern unsigned char Air_conditioner_dehumidification(void);
extern void AC_time(unsigned int i);
#endif