#include <cassert>
#include <random>
#include <numeric>
#include <algorithm>
#include "SFM.h"
#include "MyTests.h"

float CalcMeanTEST(const std::vector<float>& data, size_t idx_1, size_t idx_2)
{
    if (data.empty()) {
        return {};
    }

    const size_t size = data.size();
    if (size == 1ULL) {
        return data.front();
    }

    if (idx_1 == idx_2) {
        return CalcMeanTEST(data, 0ULL, data.size() - 1ULL);
    }

    if (idx_1 < idx_2) {
        return data.at(idx_1) / 2.f + data.at(idx_2) / 2.f;
    }

    const size_t num_2 = idx_2 + 1ULL;
    const size_t num_1 = data.size() - idx_1;
    const float div = (num_1 + num_2) * 2.f;

    return  data.at(0) * num_2 / div + data.at(idx_2) * num_2 / div + data.at(idx_1) * num_1 / div + data.back() * num_1 / div;
}

void StartTests(size_t iterationsNum)
{
    std::srand(std::time(nullptr));

    while (iterationsNum > 0ULL)
    {
        const int startValue = rand();
        const int pointsNum = rand();
        const int sectionSize = (rand() + 1) % pointsNum;

        std::vector<float> testInputData(pointsNum);
        std::iota(testInputData.begin(), testInputData.end(), startValue);

        const std::vector<float> algorithmRes = CalculateSFM(testInputData, sectionSize);

        std::vector<float> testDelta(pointsNum);
        for (int i = 0; i < pointsNum; ++i)
        {
            int firstPointIdx = i - sectionSize;
            if (firstPointIdx < 0) {
                firstPointIdx += pointsNum;
            }
            const float testValue = CalcMeanTEST(testInputData, firstPointIdx, i);
            testDelta[i] = abs(testValue - algorithmRes.at(i)) / testValue * 100.f;
        }

        const float meanDelta = std::accumulate(testDelta.begin(), testDelta.end(), 0.f) / pointsNum;
        
        float S = 0.f;
        std::for_each(testDelta.begin(), testDelta.end(), [&](float d) -> void {
            S += (meanDelta - d) * (meanDelta - d);
        });
        const int div = pointsNum - 1;
        S = div > 0 ? sqrt(S / div) : sqrt(S);

        assert(S <= 1.f);

        --iterationsNum;
    }
}