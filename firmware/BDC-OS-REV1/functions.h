#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern void HardwareInit();
extern void PrintDataSerial();
extern void SetPage();
extern void SetBatteryLoad();
extern void ledcAnalogWrite(int channel, int value, int maxvalue);

extern int DischargeState;
extern bool EnDischarge;

extern void Task1code( void * pvParameters );

#endif
