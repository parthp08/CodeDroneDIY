#ifndef STABILIZATION_H_
#define STABILIZATION_H_

#include "hardware/Attitude.h"
#include "hardware/MotorsSpeedControl.h"
#include "ControlLoop.h"
#include "hardware/Reception.h"
#include "ControlLoopConstants.h"
#include "../customLibs/Math.h"

class Stabilization : public Math {
  private:
    const float mixing = 0.5;
    static const int nbAxis = 3;
    enum AXIS { XAXIS = 0, YAXIS = 1, ZAXIS = 2 };
    int rollMotorPwr, pitchMotorPwr, yawMotorPwr = 0;
    float angularSpeedCurr[nbAxis] = {0.0, 0.0, 0.0}; // Teta speed (°/s) (only use gyro)
    float angularPosCurr[nbAxis] = {0.0, 0.0, 0.0};   // Teta position (°) (use gyro + accelero)

    /* /!\ HighPassFilterCoeff is an important coeff for complementary filter
          /!\
          Too high, position will drift, to low, there will be noise from
          accelerometer
          14-Jul-2017: loop time = 2.49ms. Gyro does not drift with coef =< 0.999
          timeCste = (coeff*dt)/(1-coeff)
           coeff = timeCste/(dt + timeCste) If we want 0.5sec, coeff = 0.5/(0.003 +
          0.5) = 0.994
        */
    float HighPassFilterCoeff = 0.9995;

    uint16_t throttle = 0;
    MotorsSpeedControl motorsSpeedControl;
    ControlLoop rollPosPID_Angle, pitchPosPID_Angle;
    ControlLoop rollSpeedPID_Angle, pitchSpeedPID_Angle;
    ControlLoop rollSpeedPID_Accro, pitchSpeedPID_Accro;
    ControlLoop yawControlLoop;
    Attitude attitude;
    Reception Rx;

  public:
    void SetMotorsPwrXConfig();
    void Init();
    void Idle();
    void Accro(float _loopTimeSec);
    void Angle(float _loopTimeSec);
    void PrintAccroModeParameters();
    void PrintAngleModeParameters();
    void ResetPID();
    void SetMotorsSpeed(volatile uint16_t *TCNTn, volatile uint16_t *OCRnA) {
        motorsSpeedControl.SetMotorsSpeed(TCNTn, OCRnA);
    }
    int GetMotorsMaxPower() {
        return motorsSpeedControl.GetMotorsMaxPower();
    }
    int GetMotorsMinPower() {
        return motorsSpeedControl.GetMotorsMinPower();
    }
    int GetMotorsMaxThrottlePercent() {
        return motorsSpeedControl.GetMotorsMaxThrottlePercent();
    }
    int GetMotorsMaxThrottle() {
        return motorsSpeedControl.GetMotorsMaxThrottle();
    }
    int GetMotorsIdleThreshold() {
        return motorsSpeedControl.GetMotorsIdleThreshold();
    }
    bool AreAttitudeOffsetsComputed() {
        return attitude.AreOffsetComputed();
    }
    void AttitudeComputeOffsets() {
        attitude.ComputeOffsets();
    }
    inline void ComputeRxImpulsionWidth() {
        Rx.GetWidth();
    }
    inline int GetFlyingMode() {
        return Rx.GetFlyingMode();
    }
    inline int GetThrottle() {
        return Rx.GetThrottle(GetMotorsMinPower(), GetMotorsMaxThrottle());
    }

    bool IsThrottleIdle() {
        return GetThrottle() < GetMotorsIdleThreshold();
    }

  private:
    void SetAngleModeControlLoopConfig();
    void SetAccroModeControlLoopConfig();
    void SetYawControlLoopConfig();
    float GetFilterTimeConstant(float _loopTimeSec);
    void GetCurrPos(float _angularPos[], float _angularSpeed[], float _loop_time);
};
#endif // STABILIZATION_H_