#include <array>
#include <vector>
#include <iostream>

using namespace std;

const int MIN_DIST = 3;

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

//template<class RandomIt>
//void merge(RandomIt aux_it, RandomIt src_it, RandomIt left, RandomIt mid, RandomIt right) {
    //// a: [left, mid)  |  b: [mid, right)
    //auto aux_left = aux_it + (left - src_it);
    //auto aux_right = aux_left + (mid - left);
    //auto aux_mid = aux_right;
    //auto aux_end = aux_it + (right - src_it);
    //for (auto it = aux_left, it_s = left; it != aux_end; ++it) {
        //*it = *it_s++;
    //}
    //for (auto it = left; it != right; ++it) {
        //if (aux_left >= aux_mid) {
            //*it = *aux_right++;
        //} else if (aux_right >= aux_end) {
            //*it = *aux_left++;
        //} else if (*aux_left < *aux_right) {
            //*it = *aux_left++;
        //} else {
            //*it = *aux_right++;
        //}
    //}
//}

template<class RandomIt>
void merge(RandomIt aux_it, RandomIt src_it, RandomIt left, RandomIt mid, RandomIt right) {
    // a: [left, mid)  |  b: [mid, right)
    RandomIt aux_start = aux_it + (left - src_it);
    RandomIt aux_end = aux_it + (right - src_it);
    RandomIt src_mid = mid;
    for (auto it = aux_start; it != aux_end; ++it) {
        if (left >= src_mid) {
            *it = *mid++;
        } else if (mid >= right) {
            *it = *left++;
        } else if (*left < *mid) {
            *it = *left++;
        } else {
            *it = *mid++;
        }
    }
    left = aux_start;
    right = aux_end;
}

template<class RandomIt>
void _merge_sort(RandomIt aux_it, RandomIt src_it, RandomIt first, RandomIt last) {
    if (first >= last-1) {
        return;
    }
    size_t dist = last - first;
    if (dist < MIN_DIST) {
        insertion_sort(first, last);
    } else {
        RandomIt mid = first + dist / 2;
        _merge_sort(src_it, aux_it, aux_it+(first-src_it), aux_it+(mid-src_it));
        _merge_sort(src_it, aux_it, aux_it+(mid-src_it), aux_it+(last-src_it));
        merge(src_it, aux_it, aux_it+(first-src_it), aux_it+(mid-src_it), aux_it+(last-src_it));
    }
}

template<class RandomIt>
void merge_sort(RandomIt first, RandomIt last) {
    if (first >= last-1) {
        return;
    }
    vector<typename iterator_traits<RandomIt>::value_type> aux(first, last);
    _merge_sort(aux.begin(), first, first, last);
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

