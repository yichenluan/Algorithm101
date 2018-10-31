#include <array>
#include <iostream>

using namespace std;

template<class RandomIt>
void shell_sort(RandomIt first, RandomIt last) {
    int length = last - first;
    if (length <= 0) {
        return;
    }
    int gap = length / 2;
    while (gap > 0) {
        for (RandomIt i = first+gap; i != last; ++i) {
            auto val = *i;
            RandomIt j = i-gap;
            while (j >= first && *j > val) {
                *(j+gap) = *j;
                j -= gap;
            }
            *(j+gap) = val;
        }
        gap /= 2;
    }
    return;
}

template<class It>
void printByIt(It begin, It end) {
    for (It curr = begin; curr != end; ++curr) {
        cout << *curr << " ";
    }
    cout << endl;
}

int main() {
    array<int, 10> unorder = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    cout << "before order: ";
    printByIt(unorder.begin(), unorder.end());

    shell_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
