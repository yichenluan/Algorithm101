#include <vector>
#include <iostream>

using namespace std;

template<class It>
void printByIt(It begin, It end);

template<class RandomIt>
typename iterator_traits<RandomIt>::value_type
choose_pivot(RandomIt first, RandomIt last) {
    RandomIt mid = first + (last - first) / 2;
    RandomIt midIt;
    if (*first > *mid) {
        if (*last > *first) {
            midIt = first;
        } else if (*last < *mid) {
            midIt = mid;
        } else {
            midIt = last;
        }
    } else {
        if (*last < *first) {
            midIt = first;
        } else if (*last > *mid) {
            midIt = mid;
        } else {
            midIt = last;
        }
    }
    swap(*first, *midIt);
    return *first;
}

template<class RandomIt>
RandomIt partition(RandomIt start, RandomIt last) {
    auto pivot = choose_pivot(start, last-1);
    RandomIt left = start;
    RandomIt right = last;
    while(left < right) {
        while (left < right && *++left < pivot);
        while (left < right && *--right > pivot);
        swap(*left, *right);
    }
    swap(*start, *--left);
    return left;
}

template<class RandomIt> 
void quick_sort(RandomIt start, RandomIt last) {
    if (start >= last-1) {
        return;
    }
    cout << "input : ";
    printByIt(start, last);
    RandomIt mid = partition(start, last);
    quick_sort(start, mid);
    quick_sort(mid+1, last);
}



template<class It>
void printByIt(It begin, It end) {
    for (It curr = begin; curr != end; ++curr) {
        cout << *curr << " ";
    }
    cout << endl;
}

int main() {
    //vector<int> unorder = {5, 4, 3, 4, 1, 4, 2};
    //vector<int> unorder = { 2, 4, 3, 1};
    //vector<int> unorder = {5, 4, 9, 4, 7, 4, 2};
    //vector<int> unorder = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> unorder = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    //vector<int> unorder = { 3, 2, 1};
    //vector<int> unorder = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    cout << "before order: ";
    printByIt(unorder.begin(), unorder.end());

    quick_sort(unorder.begin(), unorder.end());

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
