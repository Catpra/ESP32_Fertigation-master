#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

struct Task {
  uint8_t channel;
  uint16_t duration;
  uint8_t hour;
  uint8_t minute;
};

class Scheduler {
public:
  Scheduler(uint8_t numTasks);
  ~Scheduler();
  void addTask(uint8_t index, uint8_t channel, uint16_t duration, uint8_t hour, uint8_t minute);
  void checkTasks(uint8_t currentHour, uint8_t currentMinute);
private:
  Task* tasks;
  uint8_t numTasks;
};

#endif