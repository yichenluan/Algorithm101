#include <vector>
#include <iostream>

using namespace std;

template<class RandomIt>
void merge(RandomIt aux_it, RandomIt src_it, RandomIt left, RandomIt mid, RandomIt right) {
    // a: [left, mid)  |  b: [mid, right)
    auto aux_left = aux_it + (left - src_it);
    auto aux_right = aux_left + (mid - left);
    auto aux_mid = aux_right;
    auto aux_end = aux_it + (right - src_it);
    for (auto it = aux_left, it_s = left; it != aux_end; ++it) {
        *it = *it_s++;
    }
    for (auto it = left; it != right; ++it) {
        if (aux_left >= aux_mid) {
            *it = *aux_right++;
        } else if (aux_right >= aux_end) {
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
    vector<typename iterator_traits<RandomIt>::value_type> aux(first, last);
    for (int size = 1; size < last-first; size += size) {
        for (RandomIt start = first; start < last-size; start = start+size+size) {
            merge(aux.begin(), first, start, start+size, min(start+size+size, last));
        }
    }
}

template<class It>
void printByIt(It begin, It end) {
    for (It curr = begin; curr != end; ++curr) {
        cout << *curr << " ";
    }
    cout << endl;
}


int main() {
    vector<int> unorder = {9, 8, 7, 6, 5, 4, 3, 2, 1, 3, 2, 1, 3, 2, 1};
    cout << "before order: ";
    printByIt(unorder.begin(), unorder.end());

    merge_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}

