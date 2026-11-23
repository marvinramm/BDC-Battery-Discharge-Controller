#ifndef MEASURE_H
#define MEASURE_H

extern int lastStateENC_A;
extern void CheckRotaryEncoder();
extern void ReadINA219();
extern void InitINA219();

extern float shuntVoltage_mV;
extern float loadVoltage_V;
extern float busVoltage_V;
extern float current_mA;
extern float power_mW;

extern String currentDir;
extern int counter;
extern int ButtonState;

extern void ReadCellVoltages();

extern float Cell1;
extern float Cell2;
extern float Cell3;
extern float Cell4;
extern float Cell5;
extern float Cell6;

extern float Cell1Average;
extern float Cell2Average;
extern float Cell3Average;
extern float Cell4Average;
extern float Cell5Average;
extern float Cell6Average;

#endif
