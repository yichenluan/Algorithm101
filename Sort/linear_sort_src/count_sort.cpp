#include <vector>
#include <iostream>

using namespace std;

const int K = 10;


template<class It>
void printByIt(It begin, It end);

template<class It>
void printByIt(It begin, It end) {
    for (It curr = begin; curr != end; ++curr) {
        cout << *curr << " ";
    }
    cout << endl;
}


void count_sort(vector<int>& a) {
    vector<int> bucket(K, 0);
    for (auto i : a) {
        bucket[i]++;
    }
    for (int i = 1; i < 10; ++i) {
        bucket[i] += bucket[i-1];
    }
    vector<int> b(a.size());
    for (int i = K-1; i >= 0; --i) {
        int j = a[i];
        b[bucket[j]-1] = j;
        bucket[j]--;
    }
    swap(a, b);
}

int main() {
    //vector<int> unorder = {5, 4, 3, 4, 1, 4, 2};
    //vector<int> unorder = { 2, 4, 3, 1};
    //vector<int> unorder = {5, 4, 9, 4, 7, 4, 2};
    //vector<int> unorder = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<int> unorder = {9, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    //vector<int> unorder = { 3, 2, 1};
    cout << "before order: ";
    printByIt(unorder.begin(), unorder.end());

    count_sort(unorder);

    cout << "after order: ";
    printByIt(unorder.begin(), unorder.end());
}
