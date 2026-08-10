#include "metric.hpp"

#include <unistd.h>

#include <algorithm>
#include <any>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "function.hpp"

namespace analyzer::metric {
void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) { metrics.push_back(std::move(metric)); }

/**
 * @brief Вычисляет все зарегистрированные метрики для заданной функции.
 *
 * Эта функция применяет каждый метрический объект из контейнера `metrics`
 * к переданной функции `func` и собирает результаты в вектор.
 */
MetricResults MetricExtractor::Get(const function::Function &func) const {
    MetricResults results;
    results.reserve(metrics.size());

    for (const auto &metric : metrics) {
        results.push_back(metric->Calculate(func));
    }

    return results;
}

}  // namespace analyzer::metric
