# 标准库排序

我们在之前已经分析过了常见的排序算法，下面来看下 `C++` 和 `Go` 标准库中是如何实现排序算法的。

## C++ STL Sort

之前在 [Quick Sort](https://github.com/yichenluan/Algorithm101/blob/master/Sort/quick_sort.md) 中提到，当元素规模较小后，插入排序的性能会好于快速排序。STL sort 就是这么做的：

```c++
template <class _RandomAccessIter>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last) {
	if (__first != __last) {
		// 自我反省的排序
		__introsort_loop(__first, __last, __VALUE_TYPE(__first), __lg(__last - __first) * 2);
		// 插入排序
		__final_insertion_sort(__first, __last);
	}
}
```

```c++
template <class _RandomAccessIter, class _Tp, class _Size>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit) {
	// 元素规模大于 threshold(16)，才使用快排
	while (__last - __first > __stl_threshold) {
		// 自省的关键：如果递归层数达到阈值(logn)后，退化为堆排序，保证复杂度
		if (__depth_limit == 0) {
			partial_sort(__first, __last, __last);
			return;
		}
		--__depth_limit;
		_RandomAccessIter __cut =
			__unguarded_partition(__first, __last,
								 _Tp(__median(*__first, *(__first + (__last - __first)/2), *(__last - 1))));
		__introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit);
		__last = __cut;
	}
}
```
```c++
template <class _Size>
inline _Size __lg(_Size __n) {
	// 自省的关键：只允许在快排中递归 2 * logn 次
	_Size __k;
	for (__k = 0; __n != 1; __n >>= 1) ++__k;
	return __k;
}
```

具体插入排序、堆排序、快速排序的实现这里就不详细展开了，后续会将这些算法的 STL 实现添加到之前的算法解析中。

总而言之：SGI-STL 的 `Sort` 逻辑为：

1. 先判断序列大小，当大于阈值 `__stl_threshold（16)`，进入自省的快速排序算法；
2. 判断已递归层数是否达到了阈值 `2 * logn`，如果是，则退化为堆排序，保证排序时间复杂度；
3. 序列大小大于阈值，且递归层数在允许范围内，进行快速排序；
4. 小于阈值后，使用插入排序。

这里面值得注意的优化有：

1. 使用自省的快速排序，避免特殊序列导致的最坏时间复杂度；
2. 快速排序过程中，`partition` 后，只有一边进行递归，另一边通过迭代进行下一轮排序；
3. 快速排序保障了每个阈值范围之间有序后，只需要调用一次插入排序即可。


## Go STL Sort

`Go` 标准库的 `Sort` 思想与上述 `SGI-STL` 的实现类似。

```go
func Sort(data Interface) {
	n := data.Len()
	quickSort(data, 0, n, maxDepth(n))
}

// maxDepth returns a threshold at which quicksort should switch
// to heapsort. It returns 2*ceil(lg(n+1)).
// 同样的递归层数阈值
func maxDepth(n int) int {
	var depth int
	for i := n; i > 0; i >>= 1 {
		depth++
	}
	return depth * 2
}
```

```go
func quickSort(data Interface, a, b, maxDepth int) {
	// 以 12 为规模阈值，大于使用快排，小于使用希尔排序
	for b-a > 12 {
		// 自省，递归层数过大，退化为堆排序
		if maxDepth == 0 {
			heapSort(data, a, b)
			return
		}
		maxDepth--
		// 优化后的 partition，当判断重复元素很多时，变为三向切分快排
		mlo, mhi := doPivot(data, a, b)
		// Avoiding recursion on the larger subproblem guarantees
		// a stack depth of at most lg(b-a).
		if mlo-a < b-mhi {
			quickSort(data, a, mlo, maxDepth)
			a = mhi // i.e., quickSort(data, mhi, b)
		} else {
			quickSort(data, mhi, b, maxDepth)
			b = mlo // i.e., quickSort(data, a, mlo)
		}
	}
	if b-a > 1 {
		// Do ShellSort pass with gap 6
		// It could be written in this simplified form cause b-a <= 12
		// gap 为 6，1 的希尔排序
		for i := a + 6; i < b; i++ {
			if data.Less(i, i-6) {
				data.Swap(i, i-6)
			}
		}
		insertionSort(data, a, b)
	}
}
```