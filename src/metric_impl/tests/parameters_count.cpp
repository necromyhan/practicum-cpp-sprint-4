#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric_impl/parameters_count.hpp"

namespace analyzer::metric::metric_impl {

struct ParametersTestParam {
    std::string filename;
    int expected_parameters_count;
};

class ParametersCountMetricTest : public ::testing::TestWithParam<ParametersTestParam> {
protected:
    CountParametersMetric metric_;
};

TEST_P(ParametersCountMetricTest, CalculatesCorrectParametersCount) {
    const auto &param = GetParam();

    std::filesystem::path filename = std::filesystem::path(TEST_FILES_PATH) / param.filename;

    ASSERT_TRUE(std::filesystem::exists(filename)) << "Test file not found: " << filename.string();

    file::File file(param.filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions found in " << param.filename;

    int total_parameters = std::ranges::fold_left(
        functions, 0, [&](int sum, const auto &func) { return sum + std::get<int>(metric_.Calculate(func).value); });

    EXPECT_EQ(total_parameters, param.expected_parameters_count);
}

INSTANTIATE_TEST_SUITE_P(ParametersCountTests, ParametersCountMetricTest,
                         ::testing::Values(ParametersTestParam{"comments.py", 3},
                                           ParametersTestParam{"exceptions.py", 0}, ParametersTestParam{"if.py", 1},
                                           ParametersTestParam{"loops.py", 1}, ParametersTestParam{"many_lines.py", 0},
                                           ParametersTestParam{"many_parameters.py", 5},
                                           ParametersTestParam{"match_case.py", 1},
                                           ParametersTestParam{"nested_if.py", 2}, ParametersTestParam{"simple.py", 0},
                                           ParametersTestParam{"ternary.py", 1}));

}  // namespace analyzer::metric::metric_impl
