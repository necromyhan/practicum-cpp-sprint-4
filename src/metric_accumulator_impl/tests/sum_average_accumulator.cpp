#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "metric_accumulator_impl/sum_average_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class SumAverageAccumulatorTest : public ::testing::Test {
protected:
    SumAverageAccumulator accumulator_;

    metric::MetricResult CreateIntResult(const std::string &name, int value) {
        return metric::MetricResult{.metric_name = name, .value = value};
    }
};

TEST_F(SumAverageAccumulatorTest, CalcSumCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));
    accumulator_.Accumulate(CreateIntResult("test_metric", 20));
    accumulator_.Accumulate(CreateIntResult("test_metric", 30));

    accumulator_.Finalize();

    auto result = accumulator_.Get();
    EXPECT_EQ(result.sum, 10 + 20 + 30);
}

TEST_F(SumAverageAccumulatorTest, CalAvgCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));
    accumulator_.Accumulate(CreateIntResult("test_metric", 20));
    accumulator_.Accumulate(CreateIntResult("test_metric", 30));

    accumulator_.Finalize();

    auto result = accumulator_.Get();
    EXPECT_DOUBLE_EQ(result.average, (10 + 20 + 30) / 3.);
}

TEST_F(SumAverageAccumulatorTest, SingleValueCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 42));

    accumulator_.Finalize();

    auto result = accumulator_.Get();
    EXPECT_EQ(result.sum, 42);
    EXPECT_DOUBLE_EQ(result.average, 42.0);
}

TEST_F(SumAverageAccumulatorTest, GetWithoutFinalizeThrowsException) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));

    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(SumAverageAccumulatorTest, GetOnEmptyAccumulatorWithoutFinalizeThrows) {
    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(SumAverageAccumulatorTest, ResetCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));
    accumulator_.Accumulate(CreateIntResult("test_metric", 20));
    accumulator_.Finalize();

    auto result_before = accumulator_.Get();
    EXPECT_EQ(result_before.sum, 30);

    accumulator_.Reset();

    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(SumAverageAccumulatorTest, ManyValuesCorrect) {
    const int num_values = 1000;
    int expected_sum = 0;

    for (int i = 1; i <= num_values; ++i) {
        accumulator_.Accumulate(CreateIntResult("test_metric", i));
        expected_sum += i;
    }

    accumulator_.Finalize();

    auto result = accumulator_.Get();
    EXPECT_EQ(result.sum, expected_sum);
    EXPECT_DOUBLE_EQ(result.average, static_cast<double>(expected_sum) / num_values);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test