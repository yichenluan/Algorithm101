#include <array>
#include <iostream>

using namespace std;

template<class RandomIt>
void insertion_sort(RandomIt first, RandomIt last) {
    if (first >= last) {
        return;
    }
    for (RandomIt i = first+1; i != last; ++i) {
        auto val = *i;
        RandomIt j = i-1;
        while (j >= first && *j > val) {
            *(j+1) = *j;
            j--;
        }
        *(j+1) = val;
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

    insertion_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
