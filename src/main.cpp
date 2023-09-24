#include <Arduino.h>
#include "PS2X_lib.h"

#define HBRGD_IN1 9
#define HBRGD_IN2 8
#define HBRGD_IN3 11
#define HBRGD_IN4 10

#define PS2_CLK 2
#define PS2_SEL 3
#define PS2_CMD 4
#define PS2_DAT 5
#define pressure false
#define rumble false

#define DBG_MODE
//  put function declarations here:
void HbridgeStopFR()
{
    analogWrite(HBRGD_IN1, LOW);
    analogWrite(HBRGD_IN2, LOW);
    analogWrite(HBRGD_IN3, LOW);
    analogWrite(HBRGD_IN4, LOW);
}

struct
{
    signed int RightMotVal;
    signed int LeftMotVal;
    int mappedLJoyY;
    int mappedRJoyX;
    int mappedLJoyYTurning;
    int mappedRJoyXTurning;
} ContDataDecode;

PS2X Cont;

volatile struct
{
    int Lstick_Y;
    int Rstick_X;
} ContData;

void ReadController()
{
    Cont.read_gamepad(0, 0);
    ContData.Lstick_Y = Cont.Analog(PSS_LY);
    ContData.Rstick_X = Cont.Analog(PSS_RX);
}

void MapContVals(int LJoyY, int RJoyX)
{
    int TempLeftMotVal;
    int TempRightMotVal;
    ContDataDecode.mappedLJoyY = map(LJoyY, 255, 0, -200, 200);
    ContDataDecode.mappedRJoyX = map(constrain(RJoyX, 0, 255), 0, 255, -200, 200);
    ContDataDecode.mappedLJoyYTurning = map(LJoyY, 255, 0, -150, 150);
    ContDataDecode.mappedRJoyXTurning = map(constrain(RJoyX, 0, 255), 0, 255, -100, 100);
    if (ContDataDecode.mappedRJoyX) // Turning
    {
        TempLeftMotVal = ContDataDecode.mappedLJoyYTurning + ContDataDecode.mappedRJoyXTurning;
        TempRightMotVal = ContDataDecode.mappedLJoyYTurning - ContDataDecode.mappedRJoyXTurning;
    }

    else // Forward or Reverse
    {
        TempLeftMotVal = ContDataDecode.mappedLJoyY;
        TempRightMotVal = ContDataDecode.mappedLJoyY;
    }
    ContDataDecode.LeftMotVal = TempLeftMotVal;
    ContDataDecode.RightMotVal = TempRightMotVal;
}

void HbridgeDrive(int left, int right)
{
    if (left >= 0)
    {
        analogWrite(HBRGD_IN1, left);
        analogWrite(HBRGD_IN2, LOW);
    }
    else if (left < 0)
    {
        analogWrite(HBRGD_IN2, -left);
        analogWrite(HBRGD_IN1, LOW);
    }

    if (right >= 0)
    {
        analogWrite(HBRGD_IN3, right);
        analogWrite(HBRGD_IN4, LOW);
    }
    else if (right < 0)
    {
        analogWrite(HBRGD_IN4, -right);
        analogWrite(HBRGD_IN3, LOW);
    }
}

byte ContState = -1;

void setup()
{
    // put your setup code here, to run once:
#ifdef DBG_MODE
    Serial.begin(115200);
    delay(1000);
    Serial.println("bruh");
#endif
    HbridgeStopFR();
    while (ContState != 0)
    {
        ContState = Cont.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressure, rumble);
        Serial.println("????");
    }
    // cli();
    // Timer1.initialize(readintervalns);
    // Timer1.attachInterrupt(ReadController);
    // sei();
    Serial.println("done");
}

void loop()
{
    ReadController();
    MapContVals(ContData.Lstick_Y, ContData.Rstick_X);
    delay(50);
    HbridgeDrive(ContDataDecode.LeftMotVal, ContDataDecode.RightMotVal);
    Serial.print("LeftMotVal: ");
    Serial.println(ContDataDecode.LeftMotVal);
    Serial.print("RightMotVal: ");
    Serial.println(ContDataDecode.RightMotVal);

    // put your main code here, to run repeatedly:
}

// put function definitions here:
