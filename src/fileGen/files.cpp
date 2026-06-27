#include "files.hpp"

int GenerateFiles(const Result& res) {
    std::ofstream file("point.json"), bin("sums", std::ios::binary);
    if (!file.is_open() || !bin.is_open()) {
        throw std::runtime_error("error in opening file");
    }

    file << std::format("{{\"pairs\":[\n");
    for (size_t i = 0; i < res.points.size(); ++i) {
        file << std::format(
            "\t{{"
            "\"x0\":{:.10f}, "
            "\"y0\":{:.10f}, "
            "\"x1\":{:.10f}, "
            "\"y1\":{:.10f}}}",
            res.points[i].first.x, res.points[i].first.y, res.points[i].second.x, res.points[i].second.y
            );
        if (i < res.points.size() - 1)
            file << ",";
        file << "\n";
        bin.write(reinterpret_cast<const char*>(&res.sum), sizeof(res.sum));
    }
    file << "        ]}\n";
    return 0;
}
Result FormHaversine(const JsonParser& parser) {
    Result res;
    const JsonObject& pairs = std::get<JsonObject>(parser.ast.data);
    const JsonArray& arr = std::get<JsonArray>(pairs[0].val.data);
    res.points.reserve(arr.size());
    res.sum = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        const JsonObject& points = std::get<JsonObject>(arr[i].data);
        res.points.emplace_back(points);
        res.sum += res.points[i].haversineDist;
    }
    res.sum /= arr.size();
    return res;
}

double CountHaversine(const JsonParser& parser) {
    const JsonObject& pairs = std::get<JsonObject>(parser.ast.data);
    const JsonArray& arr = std::get<JsonArray>(pairs[0].val.data);
    double sum = 0;
    throughData.sumSize = arr.size() * (sizeof(JsonValue) + ( sizeof(double) * 4));
    for (size_t i = 0; i < arr.size(); ++i) {
        const JsonObject& points = std::get<JsonObject>(arr[i].data);
        sum += ReferenceHaversine(std::get<f64>(points[0].val.data), std::get<f64>(points[1].val.data),
                std::get<f64>(points[2].val.data), std::get<f64>(points[3].val.data));
    }
    sum /= arr.size();
    return sum;
}