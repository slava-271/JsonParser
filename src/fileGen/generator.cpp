#include "generator.hpp"

Result GeneratePoints(const size_t pairCount) {
    std::random_device rd;
    std::mt19937 paramGen(rd());
    std::uniform_real_distribution<f64> yDist(-90.0, 90.0), xDist(-180.0, 180.0), rangeDist(10.0, 20.0);

    std::uniform_int_distribution<size_t> clustCountDist(4, 7);
    size_t clustCount = clustCountDist(paramGen);
    std::vector<Cluster> clusters;
    clusters.reserve(clustCount);

    for (size_t i = 0; i < clustCount; ++i)
        clusters.emplace_back(xDist(paramGen), yDist(paramGen), rangeDist(paramGen));

    std::mt19937 pointGen(rd());
    std::normal_distribution<f64> normDist(0.0, 1.0);
    std::uniform_int_distribution<size_t> indxDist(1,clustCount - 1);
    Result res;
    res.sum = 0.0;
    res.points.reserve(pairCount);

    for (size_t i = 0; i < pairCount; ++i) {
        size_t index1 = indxDist(pointGen);
        size_t index2 = indxDist(pointGen);
        res.points.emplace_back(clusters[index1], clusters[index2], normDist, pointGen);
        res.sum += res.points[i].haversineDist;
    }
    res.sum /= pairCount;

    return res;

}