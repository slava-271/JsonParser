#ifndef GENERATOR_HPP
#define GENERATOR_HPP
#include "math/haversine.hpp"
#include <vector>
#include <random>
#include <utility>
#include "parser/parser.hpp"


struct Point {
    f64 x, y;
    Point(const f64 x, const f64 y) : x(x), y(y) {};
};

struct Cluster : Point {
    f64 range;
    Cluster(const f64 x, const f64 y, const f64 range) : Point(x, y), range(range) {};
};

struct PointsInfo {
    Point first, second;
    f64 haversineDist;
    PointsInfo(const Cluster& cluster0, const Cluster& cluster1, std::normal_distribution<f64>& dist, std::mt19937& gen) :
        first(cluster0.x + dist(gen) * cluster0.range, cluster0.y + dist(gen) * cluster0.range),
        second(cluster1.x + dist(gen) * cluster1.range, cluster1.y + dist(gen) * cluster1.range),
        haversineDist(ReferenceHaversine(first.x, first.y, second.x, second.y)) {};

    PointsInfo(const JsonObject& points) : first(std::get<f64>(points[0].val.data), std::get<f64>(points[1].val.data)),
        second(std::get<f64>(points[2].val.data), std::get<f64>(points[3].val.data)),
        haversineDist(ReferenceHaversine(first.x, first.y, second.x, second.y)) {};
};

struct Result {
    f64 sum;
    std::vector<PointsInfo> points;
};

Result GeneratePoints(const size_t pairCount);

#endif // GENERATOR_HPP
