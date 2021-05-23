#include "var.hpp"

std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_setpoint;
std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_error;
std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_feedback;
std::array<std::atomic<float>, LENGTH_PID_CHAIN> g_command;
std::array<std::atomic<long int>, LENGTH_PID_CHAIN> g_time;
std::array<std::atomic<bool>, LENGTH_PID_CHAIN> g_event;

std::atomic<bool> g_working;
