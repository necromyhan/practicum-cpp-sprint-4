#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric_impl/code_lines_count.hpp"

namespace analyzer::metric::metric_impl {

struct TestParam {
    std::string filename;
    int expected_total_lines;
};

class CodeLinesCountMetricTest : public ::testing::TestWithParam<TestParam> {
protected:
    CodeLinesCountMetric metric_;
};

TEST_P(CodeLinesCountMetricTest, CountsCorrectNumberOfLines) {
    const auto &param = GetParam();

    std::filesystem::path filename = std::filesystem::path(TEST_FILES_PATH) / param.filename;

    ASSERT_TRUE(std::filesystem::exists(filename)) << "Test file not found: " << filename.string();

    file::File file(param.filename);
    function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    ASSERT_FALSE(functions.empty()) << "No functions found in " << param.filename;

    int total_lines = 0;
    for (const auto &func : functions) {
        auto result = metric_.Calculate(func);
        total_lines += std::get<int>(result.value);
    }

    EXPECT_EQ(total_lines, param.expected_total_lines);
}

INSTANTIATE_TEST_SUITE_P(CodeLinesCountTests, CodeLinesCountMetricTest,
                         ::testing::Values(TestParam{"comments.py", 3}, TestParam{"exceptions.py", 7},
                                           TestParam{"if.py", 3}, TestParam{"loops.py", 6},
                                           TestParam{"many_lines.py", 11}, TestParam{"many_parameters.py", 1},
                                           TestParam{"match_case.py", 7}, TestParam{"nested_if.py", 8},
                                           TestParam{"simple.py", 5}, TestParam{"ternary.py", 1}));

}  // namespace analyzer::metric::metric_impl