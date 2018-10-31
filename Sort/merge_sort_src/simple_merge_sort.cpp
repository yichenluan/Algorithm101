#include <array>
#include <vector>
#include <iostream>

using namespace std;

template<class RandomIt>
void merge(RandomIt left, RandomIt mid, RandomIt right) {
    // a: [left, mid)  |  b: [mid, right)
    vector<typename iterator_traits<RandomIt>::value_type> aux(left, right);
    auto aux_left = aux.begin();
    auto aux_right = aux.begin() + (mid - left);
    auto aux_mid = aux_right;
    for (auto it = left; it != right; ++it) {
        if (aux_left >= aux_mid) {
            *it = *aux_right++;
        } else if (aux_right >= aux.end()) {
            *it = *aux_left++;
        } else if (*aux_left < *aux_right) {
            *it = *aux_left++;
        } else {
            *it = *aux_right++;
        }
    }
}

template<class RandomIt>
void merge_sort(RandomIt first, RandomIt last) {
    if (first >= last-1) {
        return;
    }
    RandomIt mid = first + (last - first) / 2;
    merge_sort(first, mid);
    merge_sort(mid, last);
    merge(first, mid, last);
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

    merge_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
