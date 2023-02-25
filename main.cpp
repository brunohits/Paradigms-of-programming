#include <iostream>
#include <vector>

using namespace std;

void null(){}

int *secondPartOfSort(int count, int arrCopy[], int firstIndex, int secondIndex) {
    (arrCopy[firstIndex] > arrCopy[secondIndex]) ? swap(arrCopy[firstIndex], arrCopy[secondIndex]) : null();
    arrCopy = secondIndex != count - 1 ? secondPartOfSort(count, arrCopy, firstIndex, secondIndex + 1) : arrCopy;
    return arrCopy;
}

int *firstPartOfSort(int count, int arrCopy[], int index) {
    index != count - 1 ? arrCopy = secondPartOfSort(count, arrCopy, index, index + 1), firstPartOfSort(count, arrCopy, index + 1) : nullptr;
    return arrCopy;
}

void calculateAndPrint(int quantileCount, int count, double quantileCopy[], int arrCopy[], int counter) {
    counter != quantileCount ? cout << quantileCopy[counter] << " quantile: " << arrCopy[(int) (quantileCopy[counter] * count)] << endl, calculateAndPrint(quantileCount, count, quantileCopy, arrCopy, counter + 1) : null();
}

void initializeDoubleArray(double arr[], int count, int index) {
    cin >> arr[index];
    index != count - 1 ? initializeDoubleArray(arr, count, index + 1) : null();
}

void initializeIntArray(int arr[], int count, int index) {
    cin >> arr[index];
    index != count - 1 ? initializeIntArray(arr, count, index + 1) : null();
}


void workingFunction() {
    int count, quantileCount;
    cin >> count >> quantileCount;
    int arr[count];
    double quantile[quantileCount];
    initializeIntArray(arr, count, 0);
    initializeDoubleArray(quantile, quantileCount, 0);
    firstPartOfSort(count, arr, 0);
    calculateAndPrint(quantileCount, count, quantile, arr, 0);
}

int main() {
    workingFunction();
    return 0;
}
