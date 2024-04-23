#pragma once
#include <limits>
#include <random>
#include <vector>
#include <thread>
#include <algorithm>
#include <exception>

template<typename T>
using VectIter = typename std::vector<T>::iterator;

template<typename T>
std::vector<T> GenerateData(size_t datasetSize)
{
    std::vector<T> res(datasetSize);

    static const size_t EXTRA_THREADS_NUM = static_cast<size_t>(std::thread::hardware_concurrency() - 1U);
    const size_t partSize = datasetSize / (EXTRA_THREADS_NUM + 1ULL);

    auto FillVectorWithRealRandom = [](VectIter<T> first, VectIter<T> last) -> void {
        if (std::distance(first, last) < 0) {
            throw std::runtime_error("Invalid range for vector filling");
        }

        static std::default_random_engine generator;
        static std::uniform_real_distribution<T> realDistribution(-1, 1);
        static const T MAX_VALUE{std::numeric_limits<T>::max()};
        std::generate(first, last, [&]() -> T {return realDistribution(generator) * MAX_VALUE;});
    };
    
    if (EXTRA_THREADS_NUM == 0ULL || partSize == 0ULL) {
        FillVectorWithRealRandom(res.begin(), res.end());
        return res;
    }
    
    std::vector<std::thread> threadsContainer;
    auto it = res.begin();
    for (size_t i = 0ULL; i < EXTRA_THREADS_NUM; ++i)
    {
        auto itCopy = it;
        std::advance(it, partSize);
        threadsContainer.emplace_back([=]() -> void {
            FillVectorWithRealRandom(itCopy, it);
        });
    }

    FillVectorWithRealRandom(it, res.end());

    for (std::thread& thr : threadsContainer)
    {
        thr.join();
    }

    return res;
}