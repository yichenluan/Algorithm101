#include <array>
#include <iostream>

using namespace std;

template<class RandomIt>
void bubble_sort(RandomIt first, RandomIt last) {
    if (first >= last) {
        return;
    }
    int round = 0;
    for (RandomIt i = first; i != last; ++i, ++round) {
        bool swapped = false;
        for (RandomIt curr = first; curr < last-round-1; ++curr) {
            if (*curr > *(curr+1)) {
                swap(*curr, *(curr+1));
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
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

    bubble_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
