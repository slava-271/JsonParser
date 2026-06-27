#ifndef PROFILER_HPP
#define PROFILER_HPP


#include <x86intrin.h>
#include <chrono>
#include <format>
#include <iostream>
#include <array>
#include <functional>
#include <utility>
#include <memory>

using ull = unsigned long long;

extern ull cpuFreq;

constexpr ull microCoef = 1'000'000;

ull ReadOsTimer();

ull ReadCPUTimer();

ull EstimateCPUFreq(size_t milliSeconds);

struct ProfilerAnchor;

struct Profiler {
    static inline std::array<ProfilerAnchor*, 4096> arr;
    static inline size_t length;
    static inline ProfilerAnchor* currentParent;
    static inline void set(ProfilerAnchor* element) {
        arr[length++] = element;
        return;
    }
};

struct ProfilerAnchor {
    std::string_view key;
    ull elapsed, elapsedChild, elapsedRoot, bytes;

    ProfilerAnchor(std::string_view str) : key(str), elapsed(0), elapsedChild(0), elapsedRoot(0) {
        Profiler::set(this);
    }
};

struct ProfilerBlock {
    ProfilerAnchor* parent;
    ProfilerAnchor* anchor;
    size_t start, oldElapsed;

    ProfilerBlock(ProfilerAnchor* node) : parent(Profiler::currentParent), anchor(node) {
        oldElapsed = anchor->elapsedRoot;
        Profiler::currentParent = node;
        start = ReadCPUTimer();
    }

    ~ProfilerBlock() {
        size_t elapsed = ReadCPUTimer() - start;
        Profiler::currentParent = parent;
        if (parent)
            parent->elapsedChild += elapsed;
        anchor->elapsedRoot = oldElapsed + elapsed;
        anchor->elapsed += elapsed;
    }
};

template<typename Func, typename... Args>
decltype(auto) Profile(std::string_view name, Func&& func, Args&&... args) {
    static ProfilerAnchor anchor(name);

    ProfilerBlock block(&anchor);

    return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
}

template<typename Func, typename... Args>
decltype(auto) Profile(std::string_view name, size_t size, Func&& func, Args&&... args) {
    static ProfilerAnchor anchor(name);

    ProfilerBlock block(&anchor);

    return std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
}

#endif // PROFILER_HPP
