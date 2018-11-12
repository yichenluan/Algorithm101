#include <vector>
#include <iostream>

using namespace std;

template<class It>
void printByIt(It begin, It end);

int choose_pivot(vector<int>& a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    int midIdx = lo;
    if (a[lo] < a[mid]) {
        if (a[hi] < a[lo]) {
            midIdx = lo;
        } else if(a[hi] > a[mid]) {
            midIdx = mid;
        } else {
            midIdx = hi;
        }
    } else {
        if (a[hi] > a[lo]) {
            midIdx = lo;
        } else if (a[hi] < a[mid]) {
            midIdx = mid;
        } else {
            midIdx = hi;
        }
    }
    swap(a[lo], a[midIdx]);
    return a[lo];
}

pair<int,int> partition(vector<int>& a, int lo, int hi) {
    int pivot = choose_pivot(a, lo, hi);
    cout << "pivot: " << pivot << endl;
    int lt = lo;
    int gt = hi;
    int i = lo + 1;
    while (i < gt) {
        if (a[i] < pivot) {
            swap(a[lt++], a[i++]);
        } else if (a[i] > pivot) {
            swap(a[i], a[gt--]);
        } else {
            i++;
        }
    }
    return pair<int, int>(lt, gt);
}

void quick_sort(vector<int>& a, int lo, int hi) {
    if (lo >= hi) {
        return;
    }
    cout << "enter qs: ";
    printByIt(a.begin(), a.end());

    pair<int, int> res = partition(a, lo, hi);
    quick_sort(a, lo, res.first-1);
    quick_sort(a, res.second+1, hi);
}

void quick_sort(vector<int>& a) {
    quick_sort(a, 0, a.size()-1);
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

    quick_sort(unorder);

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
