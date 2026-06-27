#include <iostream>
#include <print>
#include <filesystem>
#include "fileGen/files.hpp"
#include "profiler/profiler.hpp"
#include <optional>

ull cpuFreq = 0;

constexpr size_t testNum = 10;

ThroughputData throughData{};

std::string ReadFile(const std::string& path) {
    size_t size = std::filesystem::file_size(path);
    std::string buffer(size, '\0');
    std::ifstream jsonFile(path, std::ios::binary);
    if (!jsonFile.read(buffer.data(), size))
        throw std::runtime_error("error while opening file");
    return buffer;
}

std::string ReadThroughput(const std::string& path) {
    size_t size = std::filesystem::file_size(path);
    std::string buffer(size, '\0');
    std::ifstream jsonFile(path, std::ios::binary);
    if (!Profile("Read throughput:", &std::ifstream::read, jsonFile, buffer.data(), size) )
        throw std::runtime_error("error while opening file");
    return buffer;
}

std::string Input() {
    cpuFreq = EstimateCPUFreq(100);
    std::println("Enter a json file name");
    std::string path;

    std::cin >> path;
    return path;
}


void DetailedProfiling(const std::string& path) {
    std::string buffer = Profile("Reading File", ReadThroughput, path);
    throughData.readSize = buffer.size();

    std::vector<MetaToken> tokens = Profile("Tokenization", Tokenize, buffer);

    JsonParser parser = Profile("The whole parsing:", ProfileParse, std::move(tokens));

    throughData.sum = Profile("Sum:", CountHaversine, parser);
    return;
}

void Profiling(const std::string& path) {
    std::string buffer = Profile("Reading File", ReadThroughput, path);
    throughData.readSize = buffer.size();

    std::vector<MetaToken> tokens = Profile("Tokenization", Tokenize, buffer);

    JsonParser parser = Profile("The whole parsing:", Parse, std::move(tokens));

    throughData.sum = Profile("Sum:", CountHaversine, parser);
    return;
}

void PrintProfile() {
    for (size_t i = 0; i < Profiler::length; ++i) {
        double ms = static_cast<double>(microCoef * (Profiler::arr[i]->elapsed - Profiler::arr[i]->elapsedChild)) / (cpuFreq * 1000);
        double childMs = static_cast<double>(microCoef * Profiler::arr[i]->elapsedRoot) / (cpuFreq * 1000);
        std::string buff = "";
        if (Profiler::arr[i]->key == "Read throughput:") {
            double mb = static_cast<double>(throughData.readSize) / (1024 * 1024);
            double speed = mb / (childMs * 1024 / 1000);
            buff = std::format("total throughput: {:.3f} mb, speed: {:.3f} gb/s", mb, speed);
        } else if (Profiler::arr[i]->key == "Sum:") {
            double mb = static_cast<double>(throughData.sumSize) / (1024 * 1024);
            double speed = mb / (childMs * 1024 / 1000);
            buff = std::format("total throughput: {:.3f} mb, speed: {:.3f} gb/s", mb, speed);
        }

        std::println("{} {:.4f} ms the whole function, {:.4f} ms with children, {}", Profiler::arr[i]->key, ms, childMs, buff);
    }
    std::println("Haversine sum: {}", throughData.sum);
    return;
}

int main()
{
    std::println("Enter a number. "
    "1 -- for generation. 2 -- for testing lexer and parser, 3 -- for counting haversine in input json"
     ", 4 - estimate CPU frequency. 5 -- detailed profiling, 6 -- profiling, 7 -- rdtsc check for the whole program");
    size_t choice;
    std::cin >> choice;
    switch (choice) {
    case 1: {
        std::println("Enter a pair amount");
        size_t num;
        std::cin >> num;
        Result res = GeneratePoints(num);
        std::println("average sum = {:.10f}", res.sum);
        GenerateFiles(res);
        break;
    }
    case 2: {
        std::println("Enter a json file name");
        std::string path;
        std::cin >> path;
        std::string buffer = ReadFile(path);
        std::vector<MetaToken> tokens = Tokenize(buffer);
        std::ofstream outputTest("tokens.txt");

        for (auto c: tokens) {
            std::println("Token is");
            outputTest << "Token is\n";
            std::cout << tokArr[c.token] << "\n";
            outputTest << tokArr[c.token] << "\n";
            std::println("Str is");
            outputTest << "Str is\n";
            std::cout << c.str << "\n";
            outputTest << c.str << "\n\n";
        }

        JsonParser parser = Parse(std::move(tokens));

        parser.JsonPrint();
        break;
    }
    case 3: {
        std::println("Enter a json file name");
        std::string path;
        std::cin >> path;

        std::string buffer = ReadFile(path);
        JsonParser parser = ParseJson(buffer);
        Result res = FormHaversine(parser);

        std::println("average sum = {:.10f}", res.sum);
        break;
    }
    case 4: {
        std::println("Enter a number of milliseconds");
        size_t milliSec;
        std::cin >> milliSec;
        EstimateCPUFreq(milliSec);
        break;
    }
    case 5: {
        std::string path = Input();
        Profile("the whole programm:", DetailedProfiling, path);
        PrintProfile();
        break;
    }
    case 6: {
        std::string path = Input();
        Profile("the whole programm:", Profiling, path);
        PrintProfile();
        break;
    }
    case 7: {
        std::string path = Input();
        size_t start = ReadCPUTimer();

        std::string buffer = ReadFile(path);
        JsonParser parser = ParseJson(buffer);
        double res = CountHaversine(parser);

        size_t end = ReadCPUTimer();

        double ms = static_cast<double>(microCoef * (end - start)) / (cpuFreq * 1000);
        std::println("time is: {:.4f}", ms);
        break;
    }
    case 8: {

    }
    }
    return 0;
}
