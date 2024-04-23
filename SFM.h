#pragma once
#include <thread>
#include <exception>
#include <vector>
#include <iostream>

template<typename T>
T CalcMean(const std::vector<T>& data, size_t idx_1, size_t idx_2)
{
    static_assert(std::is_floating_point<T>::value, "Only for real numbers");

    if (data.empty()) {
        return {};
    }

    const size_t size = data.size();
    if (size == 1ULL) {
        return data.front();
    }
    
    if (idx_1 == idx_2) {
        return CalcMean(data, 0ULL, size - 1ULL);
    }

    const size_t num = idx_2 > idx_1 ? (idx_2 - idx_1 + 1ULL) : ((idx_2 + 1ULL) + (size - idx_1));
    
    T res = 0;
    while (idx_2 != idx_1)
    {
        res += data.at(idx_2) / num;
        idx_2 = (idx_2 == 0ULL ? size : idx_2) - 1ULL;
    }
    res += data.at(idx_2) / num;

    return res;
}

template<typename T>
std::vector<T> CalculateSFM(const std::vector<T>& inputData, size_t sectionSize)
{
    std::vector<T> res(inputData);
    if (sectionSize <= 1ULL || res.empty()) {
        return res;
    }
    
    const size_t datasetSize = inputData.size();
    if (sectionSize > datasetSize) {
        const T meanValue = CalcMean<T>(inputData, 0ULL, datasetSize - 1ULL);
        res = std::vector<T>(res.size(), meanValue);
        return res;
    }

    static const size_t EXTRA_THREADS_NUM = static_cast<size_t>(std::thread::hardware_concurrency() - 1ULL);

    auto CalcResultPart = [&](size_t index1, size_t index2) -> void {
        if (index1 > index2) {
            throw std::runtime_error("Invalid range for SFM calculation");
        }

        int firstElelementIndex = index2 - sectionSize + 1ULL;
        if (firstElelementIndex < 0) {
            firstElelementIndex += datasetSize;
        }

        res[index2] = CalcMean<T>(inputData, firstElelementIndex, index2);
        --index2;

        while (index2 >= index1)
        {
            const size_t outputElementIndex = index2 + 1ULL;
            --firstElelementIndex;
            if (firstElelementIndex == -1) {
                firstElelementIndex = datasetSize - 1ULL;
            }
            
            res[index2] = res[outputElementIndex] - inputData[outputElementIndex] / sectionSize + inputData[firstElelementIndex] / sectionSize;
            if (index2 == 0ULL) {
                break;
            } else {
                --index2;
            }
        }
    };

    const size_t dataPartSize = datasetSize / (EXTRA_THREADS_NUM + 1ULL);
    if (dataPartSize == 0ULL) {
        CalcResultPart(0ULL, datasetSize - 1ULL);
        return res;
    }

    size_t idx1 = 0ULL;
    std::vector<std::thread> threadsContainer;
    for (size_t i = 0ULL; i < EXTRA_THREADS_NUM; ++i)
    {
        threadsContainer.emplace_back([=]() -> void {
            CalcResultPart(idx1, idx1 + dataPartSize - 1ULL);
        });
        idx1 += dataPartSize - 1ULL;
    }

    CalcResultPart(idx1, datasetSize - 1ULL);

    for (std::thread& thr : threadsContainer)
    {
        thr.join();
    }

    return res;
}