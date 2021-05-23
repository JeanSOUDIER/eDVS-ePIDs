#ifndef VAR_H
#define VAR_H

#include <atomic>
#include <array>

#define LENGTH_PID_CHAIN 2

extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_setpoint;
extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_error;
extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_feedback;
extern std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_command;
extern std::array<std::atomic<long int>, LENGTH_PID_CHAIN> g_time;
extern std::array<std::atomic<bool>, LENGTH_PID_CHAIN> g_event;

extern std::atomic<bool> g_working;

#endif //VAR_H
