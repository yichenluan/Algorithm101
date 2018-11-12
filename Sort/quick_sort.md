# 快速排序（Quick Sort）

[快速排序](https://en.wikipedia.org/wiki/Quicksort) 也是一种分治思想的排序算法。它将一个数组分成两个子数组，将两部分独立地排序，当两个子数组都有序了，整个数组也就有序了。其中，最关键的地方在于当切分（partition）时，保证切分元素在切分后放在了正确的位置，同时使得左子数组都小于等于切分元素，右子数组都大于大于切分元素。之后，通过递归排序各个子数组即可。

## 基本实现

C++ 实现如下：

```c++
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
    RandomIt mid = partition(start, last);
    quick_sort(start, mid);
    quick_sort(mid+1, last);
}
```

其实要写对快排不是一件那么简单的事，这里总结几个要着重注意的点：

1. 递归退出条件；
2. partition 退出条件；
3. choose_pivot 必须做额外工作，以保证每次 partition pivot 元素在正确位置上（上面的例子中，选中 pivot 后把他交换到最左边）；
4. 思考 partition 过程中，移动指针遇到与 pivot 相同的元素时，是该停下来进行交换还是继续移动？对于重复值大的数组而言，必须采取停下来交换。因为虽然这样子多了很多等值元素的交换，但避免了左右子数组极度不平衡的情况。
5. 仔细思考 partition 结束后，pivot 的正确位置在哪（也就是将 pivot 与哪个位置进行交换）。

下面分析下快排：

1. 时间复杂度
	
	不难想到，快排的时间复杂度和 pivot 的选择有关。
	
	- 最好情况 O(nlogn)：pivot 使得 partition 后的左右子数组等大小
	- 最坏情况 O(n^2)：pivot 使得 partition 后总有一个子数组为空
	- 平均情况 O(nlogn)

2. 空间复杂度

	快排是原地排序算法。
	
3. 稳定性

	快排需要大幅度交换元素，是不稳定的排序算法。
	
## 算法改进

快排有一些非常有效的改进措施：

1. 切换到插入排序

	类似于之前提供的归并排序，当数组较小时，快速排序性能不如插入排序，并且数组很小的时候，递归的快排也会不断的调用自己，增加了额外开销。所以，当未排序数组缩小到某个规模后，可以替换为插入排序。

2. 三取样切分

	快排的性能与 pivot 的选择息息相关，最好是每次 pivot 都选择为数组的中位数，这样能保证 partition 后左右子数组元素个数相等。但是计算中位数的代价不容忽视，一个好的方法是取数组的 3 个元素，然后选择其中的中位数作为 pivot，效果最好。上面的代码已经应用了该方案。

3. 处理大量重复值元素的数组

	对于大量重复元素的数组，快排可以有额外的改进技巧。考虑一个数组元素全部相同，使用标准快排的时间复杂度为 O(nlogn），但是使用 `三向切分快排` 时间复杂度能够降为 O(n)，具体代码见下。
	
## 迭代快排

快排可以使用非递归的方式实现，具体来说，我们需要手动使用一个栈，来保存待排序部分，代码如下：

```c++
// ... 省略部分代码，与上同

template<class RandomIt>
void quick_sort(RandomIt start, RandomIt last) {
    if (start >= last-1) {
        return;
    }
    typedef pair<RandomIt, RandomIt> itPair;

    stack<itPair> position;
    position.push(itPair(start, last));
    while (!position.empty()) {
        itPair currPos = position.top();
        position.pop();
        RandomIt left = currPos.first;
        RandomIt right = currPos.second;
        if (left >= right-1) {
            continue;
        }
        RandomIt mid = partition(left, right);
        position.push(itPair(left, mid));
        position.push(itPair(mid+1, right));
    }
}
```

## 另一种 Partition

说实话，没有提前准备，一次性写对 `partition` 不是件容易的事，不过，有一种我称之为 `beautiful partition` 的方法：

```c++
int partition(vector<int>& a, int lo, int hi) {
    int pivot = choose_pivot(a, lo, hi);
    int left = lo;
    for (int i = lo+1; i <= hi; ++i) {
        if (a[i] < pivot) {
            swap(a[++left], a[i]);
        }
    }
    swap(a[lo], a[left]);
    return left;
}
```

## 三向切分快排

![](http://p890o7lc8.bkt.clouddn.com/20181112165605.png)

三向切分快排的思想是，从左到右遍历数组一次，维护一个指针 `lt` 使得 `a[lo, lt-1]` 中的元素都小于 `v`，一个指针 `gt` 使得 `a[gt+1, hi]` 中的元素都大于 `v`，一个指针 `i` 使得 `a[lt, i-1]` 中的元素都等于 `v`，`a[i, gt]` 中的元素还未确定。于是，我们遍历每个元素进行三向比较：

- `a[i]` 小于 `v`，将 `a[lt]` 和 `a[i]` 交换，将 `lt` 和 `i` 加一；
- `a[i]` 大于 `v`，将 `a[gt]` 和 `a[i]` 交换，将 `gt` 减一；
- `a[i]` 等于 `v`，将 `i` 加一。

代码如下：

```c++
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

    pair<int, int> res = partition(a, lo, hi);
    quick_sort(a, lo, res.first-1);
    quick_sort(a, res.second+1, hi);
}

void quick_sort(vector<int>& a) {
    quick_sort(a, 0, a.size()-1);
}
```