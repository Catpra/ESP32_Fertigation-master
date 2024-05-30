#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "FertigationOutput.h"

#define MAX_TASKS 23
struct Task {
  uint8_t pin;
  uint16_t duration;
  uint8_t hour;
  uint8_t minute;
  bool executed;
};

class Scheduler {
public:
  Scheduler(FertigationOutput& fertigationOutput);
  void addTask(uint8_t pin, uint16_t duration, uint8_t hour, uint8_t minute);
  void checkTasks(uint8_t currentHour, uint8_t currentMinute);
private:
  Task m_tasks[MAX_TASKS];
  uint8_t m_taskCount;
  FertigationOutput& m_fertigationOutput;
};

#endif