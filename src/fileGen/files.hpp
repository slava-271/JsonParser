#ifndef FILES_H
#define FILES_H

#include <fstream>
#include <format>
#include "generator.hpp"

struct ThroughputData {
    double sum;
    size_t readSize, sumSize;
};
extern ThroughputData throughData;

Result FormHaversine(const JsonParser& parser);

int GenerateFiles(const Result& res);
double CountHaversine(const JsonParser& parser);

#endif // FILES_H
