#include <array>
#include <iostream>

using namespace std;

template<class RandomIt>
void selection_sort(RandomIt first, RandomIt last) {
    if (first >= last) {
        return;
    }
    for (RandomIt i = first; i != last-1; ++i) {
        RandomIt min_it = i;
        for (RandomIt j = i; j != last; ++j) {
            min_it = (*j < *min_it)? j: min_it;
        }
        swap(*i, *min_it);
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

    selection_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
