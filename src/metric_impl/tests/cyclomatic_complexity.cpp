#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"

namespace analyzer::metric::metric_impl {

struct CyclomaticTestParam {
    std::string filename;
    int expected_complexity;
};

class CyclomaticComplexityMetricTest : public ::testing::TestWithParam<CyclomaticTestParam> {
protected:
    CyclomaticComplexityMetric metric_;
};

TEST_P(CyclomaticComplexityMetricTest, CalculatesCorrectComplexity) {
    const auto &param = GetParam();

    std::filesystem::path filename = std::filesystem::path(TEST_FILES_PATH) / param.filename;

    ASSERT_TRUE(std::filesystem::exists(filename)) << "Test file not found: " << filename.string();

    file::File file(param.filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions found in " << param.filename;

    int total_complexity = 0;
    for (const auto &func : functions) {
        auto result = metric_.Calculate(func);
        total_complexity += std::get<int>(result.value);
    }

    EXPECT_EQ(total_complexity, param.expected_complexity);
}

INSTANTIATE_TEST_SUITE_P(CyclomaticComplexityTests, CyclomaticComplexityMetricTest,
                         ::testing::Values(CyclomaticTestParam{"comments.py", 1},
                                           CyclomaticTestParam{"exceptions.py", 4}, CyclomaticTestParam{"if.py", 2},
                                           CyclomaticTestParam{"loops.py", 4}, CyclomaticTestParam{"many_lines.py", 2},
                                           CyclomaticTestParam{"many_parameters.py", 2},
                                           CyclomaticTestParam{"match_case.py", 4},
                                           CyclomaticTestParam{"nested_if.py", 4}, CyclomaticTestParam{"simple.py", 2},
                                           CyclomaticTestParam{"ternary.py", 3}));

}  // namespace analyzer::metric::metric_impl