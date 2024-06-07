#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <chrono>
#include <cassert>
#include <sstream>

/**
 * @brief Інтерфейс для сортування
 */
class ISort {
public:
    /**
     * @brief Сортує дані
     * @param data Вектор даних для сортування
     */
    virtual void sort(std::vector<int>& data) = 0;
    virtual ~ISort() = default;
};

/**
 * @brief Клас для послідовного сортування методом Merge Sort
 */
class SequentialMergeSort : public ISort {
public:
    void sort(std::vector<int>& data) override {
        mergeSort(data, 0, data.size() - 1);
    }

private:
    void mergeSort(std::vector<int>& data, int left, int right) {
        if (left < right) {
            int middle = left + (right - left) / 2;
            mergeSort(data, left, middle);
            mergeSort(data, middle + 1, right);
            merge(data, left, middle, right);
        }
    }

    void merge(std::vector<int>& data, int left, int middle, int right) {
        int n1 = middle - left + 1;
        int n2 = right - middle;
        std::vector<int> leftArray(n1);
        std::vector<int> rightArray(n2);

        for (int i = 0; i < n1; ++i)
            leftArray[i] = data[left + i];
        for (int i = 0; i < n2; ++i)
            rightArray[i] = data[middle + 1 + i];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftArray[i] <= rightArray[j]) {
                data[k] = leftArray[i];
                i++;
            } else {
                data[k] = rightArray[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            data[k] = leftArray[i];
            i++;
            k++;
        }

        while (j < n2) {
            data[k] = rightArray[j];
            j++;
            k++;
        }
    }
};

/**
 * @brief Клас для паралельного сортування методом Merge Sort
 */
class ParallelMergeSort : public ISort {
public:
    void sort(std::vector<int>& data) override {
        parallelMergeSort(data, 0, data.size() - 1);
    }

private:
    void parallelMergeSort(std::vector<int>& data, int left, int right) {
        if (left < right) {
            int middle = left + (right - left) / 2;
            std::future<void> leftFuture = std::async(std::launch::async, &ParallelMergeSort::parallelMergeSort, this, std::ref(data), left, middle);
            parallelMergeSort(data, middle + 1, right);
            leftFuture.get();
            merge(data, left, middle, right);
        }
    }

    void merge(std::vector<int>& data, int left, int middle, int right) {
        int n1 = middle - left + 1;
        int n2 = right - middle;
        std::vector<int> leftArray(n1);
        std::vector<int> rightArray(n2);

        for (int i = 0; i < n1; ++i)
            leftArray[i] = data[left + i];
        for (int i = 0; i < n2; ++i)
            rightArray[i] = data[middle + 1 + i];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftArray[i] <= rightArray[j]) {
                data[k] = leftArray[i];
                i++;
            } else {
                data[k] = rightArray[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            data[k] = leftArray[i];
            i++;
            k++;
        }

        while (j < n2) {
            data[k] = rightArray[j];
            j++;
            k++;
        }
    }
};

/**
 * @brief Функція для вимірювання часу виконання сортування
 * @param sorter Об'єкт класу, що реалізує інтерфейс ISort
 * @param data Вектор даних для сортування
 */
void benchmarkSort(ISort& sorter, std::vector<int>& data) {
    auto start = std::chrono::high_resolution_clock::now();
    sorter.sort(data);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
}

/**
 * @brief Власні юніт тести для сортування
 */
void runTests() {
    std::vector<int> data1 = {5, 2, 9, 1, 5, 6};
    std::vector<int> expected1 = {1, 2, 5, 5, 6, 9};

    SequentialMergeSort seqSorter;
    seqSorter.sort(data1);
    assert(data1 == expected1);
    std::cout << "SequentialMergeSort test passed!" << std::endl;

    std::vector<int> data2 = {5, 2, 9, 1, 5, 6};
    ParallelMergeSort parSorter;
    parSorter.sort(data2);
    assert(data2 == expected1);
    std::cout << "ParallelMergeSort test passed!" << std::endl;
}

/**
 * @brief Основна функція для виконання вимірювання часу та запуску власних юніт тестів
 * @return int
 */
int main() {
    runTests();

    // Введення даних користувачем
    std::vector<int> userData;
    std::cout << "Enter numbers to sort, separated by spaces (press Enter to finish): ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    int number;
    while (iss >> number) {
        userData.push_back(number);
    }

    // Виконання послідовного сортування
    SequentialMergeSort seqSorter;
    std::vector<int> dataCopy = userData;
    std::cout << "Sequential Sort:" << std::endl;
    benchmarkSort(seqSorter, dataCopy);
    std::cout << "Sorted data: ";
    for (int num : dataCopy) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // Виконання паралельного сортування
    ParallelMergeSort parSorter;
    dataCopy = userData;
    std::cout << "Parallel Sort:" << std::endl;
    benchmarkSort(parSorter, dataCopy);
    std::cout << "Sorted data: ";
    for (int num : dataCopy) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
