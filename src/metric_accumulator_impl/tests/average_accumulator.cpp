#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "metric_accumulator_impl/average_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

class AverageAccumulatorTest : public ::testing::Test {
protected:
    AverageAccumulator accumulator_;

    metric::MetricResult CreateIntResult(const std::string &name, int value) {
        return metric::MetricResult{.metric_name = name, .value = value};
    }
};

TEST_F(AverageAccumulatorTest, CalcCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));
    accumulator_.Accumulate(CreateIntResult("test_metric", 20));
    accumulator_.Accumulate(CreateIntResult("test_metric", 30));

    accumulator_.Finalize();

    EXPECT_DOUBLE_EQ(accumulator_.Get(), (10 + 20 + 30) / 3.);
}

TEST_F(AverageAccumulatorTest, SingleValueCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 42));

    accumulator_.Finalize();

    EXPECT_DOUBLE_EQ(accumulator_.Get(), 42.0);
}

TEST_F(AverageAccumulatorTest, GetWithoutFinalizeThrowsException) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));

    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(AverageAccumulatorTest, GetOnEmptyAccumulatorWithoutFinalizeThrows) {
    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(AverageAccumulatorTest, ResetCorrect) {
    accumulator_.Accumulate(CreateIntResult("test_metric", 10));
    accumulator_.Accumulate(CreateIntResult("test_metric", 20));
    accumulator_.Finalize();

    double result_before = accumulator_.Get();
    EXPECT_DOUBLE_EQ(result_before, 15.0);

    accumulator_.Reset();

    EXPECT_THROW(accumulator_.Get(), std::runtime_error);
}

TEST_F(AverageAccumulatorTest, ManyValuesCorrect) {
    const int num_values = 1000;
    int expected_sum = 0;

    for (int i = 1; i <= num_values; ++i) {
        accumulator_.Accumulate(CreateIntResult("test_metric", i));
        expected_sum += i;
    }

    accumulator_.Finalize();

    EXPECT_DOUBLE_EQ(accumulator_.Get(), static_cast<double>(expected_sum) / num_values);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl