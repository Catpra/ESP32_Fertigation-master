#include "Scheduler.h"
#include "FertigationOutput.h"

Scheduler::Scheduler(uint8_t numTasks) {
  this->numTasks = numTasks;
  this->tasks = new Task[numTasks];
}

Scheduler::~Scheduler() {
  delete[] this->tasks;
}

void Scheduler::addTask(uint8_t index, uint8_t channel, uint16_t duration, uint8_t hour, uint8_t minute) {
  if (index < numTasks) {
    hour = hour % 24; // Ensure hour is within 0-23 range
    tasks[index] = {channel, duration, hour, minute};
  }
}

#include "FertigationOutput.h" // Add missing import

extern uint8_t arOutputsPin[]; // Add missing declaration

void Scheduler::checkTasks(uint8_t currentHour, uint8_t currentMinute) {
  for (uint8_t i = 0; i < numTasks; i++) {
    if (tasks[i].hour == currentHour && tasks[i].minute == currentMinute) {
      digitalWrite(arOutputsPin[tasks[i].channel], HIGH);
      delay(tasks[i].duration);
      digitalWrite(arOutputsPin[tasks[i].channel], LOW);
    }
  }
}

