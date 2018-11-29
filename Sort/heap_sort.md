# 堆排序（Heap Sort）

[堆排序](https://en.wikipedia.org/wiki/Heapsort) 是利用优先队列实现的排序方法。最核心的思想是将所有元素构成一个最小优先队列，然后每次调用删除最小元素操作即可获得有序数组。用无序数组实现的优先队列相当于是一次选择排序。但是如果使用基于堆的优先队列，就得到了堆排序。

## 排序思想

堆排序的过程分为两个步骤：

1. 堆的构造
2. 下沉排序

在堆的构造过程中，一种高效的方法是至底向上使用 `sink()` 遍历一遍数组，即可完成构造。

构造完毕后，交换堆顶（最大元素）和最右侧元素，此次尾部有序，然后再对交换而来的堆顶元素下沉 `sink()`，得到有效的堆，继续循环，即可完成排序。

关于堆的更多信息，可以阅读[优先队列](还没写)一节。

## C++ Implementation

```c++
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
```

## Go Implementation

Go 的 `sort`标准库中实现了 `heap sort`，代码与上面的实现思想一致，具体如下：

```go
// siftDown implements the heap property on data[lo, hi).
// first is an offset into the array where the root of the heap lies.
func siftDown(data Interface, lo, hi, first int) {
	root := lo
	for {
		child := 2*root + 1
		if child >= hi {
			break
		}
		if child+1 < hi && data.Less(first+child, first+child+1) {
			child++
		}
		if !data.Less(first+root, first+child) {
			return
		}
		data.Swap(first+root, first+child)
		root = child
	}
}

func heapSort(data Interface, a, b int) {
	first := a
	lo := 0
	hi := b - a

	// Build heap with greatest element at top.
	for i := (hi - 1) / 2; i >= 0; i-- {
		siftDown(data, i, hi, first)
	}

	// Pop elements, largest first, into end of data.
	for i := hi - 1; i >= 0; i-- {
		data.Swap(first, first+i)
		siftDown(data, lo, i, first)
	}
}
```