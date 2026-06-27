#include "profiler.hpp"

ull ReadOsTimer() {
    using namespace std::chrono;

    auto now = steady_clock().now();
    auto duration = now.time_since_epoch();
    return duration_cast<microseconds>(duration).count();
}

ull ReadCPUTimer() {
    return _rdtsc();
}

ull EstimateCPUFreq(size_t milliSeconds) {
    ull cpuStart = ReadCPUTimer();
    ull osStart = ReadOsTimer();
    ull osElapse = 0;
    ull osEnd = 0;
    ull microSeconds = milliSeconds * 1000;
    while (osElapse < microSeconds) {
        osEnd = ReadOsTimer();
        osElapse = osEnd - osStart;
    }

    ull cpuEnd = ReadCPUTimer();
    ull cpuElapse = cpuEnd - cpuStart;
    ull cpuFreq = 0;
    if (osElapse)
        cpuFreq = cpuElapse * microCoef / osElapse;

    std::cout << std::format(
        "OS Timer: {:d} -> {:d} = {:d} elapsed \n"
        "OS seconds: {:5f}\n"
        "CPU Timer: {:d} -> {:d} = {:d} elapsed\n"
        "CPU Freq: {:d} (guessed)\n",
        osStart, osEnd, osElapse, static_cast<double>(osElapse) / microCoef, cpuStart, cpuEnd, cpuElapse, cpuFreq
        );
    return cpuFreq;

}


