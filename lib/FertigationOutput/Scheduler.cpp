#include "Scheduler.h"

Scheduler::Scheduler(FertigationOutput& fertigationOutput) : m_taskCount(0), m_fertigationOutput(fertigationOutput) {}

void Scheduler::addTask(uint8_t pin, uint16_t duration, uint8_t hour, uint8_t minute) {
  if (m_taskCount < MAX_TASKS) {
    m_tasks[m_taskCount] = {pin, duration, hour, minute, false};
    m_taskCount++;
    m_fertigationOutput.addSolenoidOutput(pin, duration);
  }
}

void Scheduler::checkTasks(uint8_t currentHour, uint8_t currentMinute) {
  for (uint8_t i = 0; i < m_taskCount; i++) {
    if (!m_tasks[i].executed && m_tasks[i].hour == currentHour && m_tasks[i].minute == currentMinute) {
      m_fertigationOutput.addSolenoidOutput(m_tasks[i].pin, m_tasks[i].duration);
      m_tasks[i].executed = true;
    }
  }
}