#pragma once
#include <ctime>
#include <cstdlib>

template <typename T>
void shuffle(std::vector<T>& array) {
    static bool isInitialized = false;
    if (!isInitialized) {
        srand(time(nullptr));
        isInitialized =  true;
    }
    for(int i = array.size()-1; i > 0; i--) {
        auto randomIndex = rand() % (i + 1);
        auto temp = array[i];
        array[i] = array[randomIndex];
        array[randomIndex] = temp;
    }
}