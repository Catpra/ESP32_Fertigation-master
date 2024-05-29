#include <Arduino.h>
#include <Ticker.h>
#define MAX_OUTPUTS 8
#define ON 1
#define OFF 0
#define MOTORPUMP_DELAY 500

class FertigationOutput {
  public:
    FertigationOutput();
    void setMotorPumpPin(uint8_t nPin, u_int16_t nOnDelay, u_int16_t nOffDelay) {
      m_nMotorPumpPin = nPin;
      m_nMotorPumpOnDelay = nOnDelay;
      m_nMotorPumpOffDelay = nOffDelay;
    }
    void addSolenoidOutput(uint8_t nPin, uint16_t nDuration);
    void begin() {
      initOutputs();
    }
    void start();
    void update();

protected:
     static void setMotorPumpOn(FertigationOutput* p){
        if (p->m_nMotorPumpPin != -1)
          digitalWrite(p->m_nMotorPumpPin, ON);
     }
     
     static void setMotorPumpOff(FertigationOutput* p){
        if (p->m_nMotorPumpPin != -1)
          digitalWrite(p->m_nMotorPumpPin, OFF);
     }

  private:
     void initOutputs();

    bool m_fStart;
    Ticker m_ticker;
    int8_t  m_nMotorPumpPin=-1;
    u_int16_t m_nMotorPumpOnDelay;
    u_int16_t m_nMotorPumpOffDelay;
    uint8_t m_arOutputPin[MAX_OUTPUTS];
    uint16_t m_arOutputDuration[MAX_OUTPUTS];
    uint8_t m_nOutputCount;
    uint8_t m_nCurrentIndex;
    uint32_t m_nLastMillis;
};