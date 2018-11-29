#include <vector>
#include <iostream>

using namespace std;


template<class It>
void printByIt(It begin, It end);

template<class It>
void printByIt(It begin, It end) {
    for (It curr = begin; curr != end; ++curr) {
        cout << *curr << " ";
    }
    cout << endl;
}

void sink(vector<int>& a, int k, int N) {
    int child = 2 * (k+1) - 1;
    while (child <= N) {
        if (child < N && a[child] < a[child+1]) {
            child++;
        }
        if (a[k] >= a[child]) {
            break;
        }
        swap(a[k], a[child]);
        k = child;
        child = 2 *(k+1) - 1;
    }
}

void heap_sort(vector<int>& a) {
    int N = a.size() - 1;
    for (int k = N/2; k >= 0; --k) {
        sink(a, k, N);
    }
    while (N > 0) {
        swap(a[0], a[N--]);
        sink(a, 0, N);
    }
}

int main() {
    //vector<int> unorder = {5, 4, 3, 4, 1, 4, 2};
    //vector<int> unorder = { 2, 4, 3, 1};
    //vector<int> unorder = {5, 4, 9, 4, 7, 4, 2};
    //vector<int> unorder = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    //vector<int> unorder = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    //vector<int> unorder = { 3, 2, 1};
    vector<int> unorder;
    cout << "before order: ";
    printByIt(unorder.begin(), unorder.end());

    heap_sort(unorder);

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
