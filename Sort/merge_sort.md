# 归并排序（Merge Sort）

[归并排序](https://en.wikipedia.org/wiki/Merge_sort)是分治算法的典型代表。

![](https://upload.wikimedia.org/wikipedia/commons/e/e6/Merge_sort_algorithm_diagram.svg)

上图很好的解释了归并排序算法的工作机理。

## Top-Down Implementation

我们用递归可以很容易的实现最基础的自顶向下归并排序：

```c++
// C++
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
```

但是这段代码有许多可以优化的地方：

1. 对小规模子数组使用插入排序。

	在小规模文章中递归方法调用的太过频繁，性能比不过一般的初级排序算法，之前也分析过，插入排序是初级排序算法中最合适的（因为冒泡多了更多次比较操作以及更复杂的交换操作）。

2. 当左右半边已经有序的情况下，无需进行归并。
3. 避免每次归并时初始化一个辅助数组，而是全局使用唯一一个辅助数组用以暂存数据。

	实际上，这一步还可以避免每次归并前都将待归并元素 copy 到全局辅助数组中，具体方法是每次归并时交换输入数组和输出数组，也就是说每次归并过程中，直接把序列归并到辅助数组中，然后返回辅助数组，最后合并的时候，再合并回输入数组。
	
下面是应用优化 1 和优化 3 的代码：

```c++
// C++
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
        // 注意这里入参的处理，是保证最后归并回到原数组的关键
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
```

```go
// Go
// 大同小异，暂略
```

## Bottom-Up Implementation

Top-Down 的归并排序是递归算法的一种应用，主要思想就是由大至小，最后再逐渐合并。实现归并排序的另一种思想是自底向上的，先归并小数组，再归并上一轮归并后的数组，知道整个数组归并完毕。实现如下：

```c++
// C++
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
```

```go
// Go
// 暂略
```

1. 时间复杂度

	- 最好情况 O(nlogn)
	- 最坏情况 O(nlogn)
	- 平均情况 O(nlogn)

2. 空间复杂度

	对于数组的归并排序，空间复杂度为 O(n)
	
3. 稳定性

	是否稳定关键在于 `merge` 函数，我们的实现是稳定排序算法。
	
## Merge Sort in Linked List

以上描述的都是在数组上应用归并排序，实际上由于归并排序的空间复杂度问题以及常数项要大于快速排序，我们很少在非外部排序的场景下应用归并排序。

不过，在链表场景下，归并排序是排序的不二之选。下面让我们来欣赏`std::list`的`list::sort`的实现：

```c++
// sgi-stl
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::sort()
{
  	// Do nothing if the list has length 0 or 1.
  	if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
    	list<_Tp, _Alloc> __carry;
    	list<_Tp, _Alloc> __counter[64];
    	int __fill = 0;
    	while (!empty()) {
    		// 取待排序队首元素放到 carry 中
      		__carry.splice(__carry.begin(), *this, begin());
      		int __i = 0;
      		while(__i < __fill && !__counter[__i].empty()) {
      			// 如果 counter 当前下标有已排序列表，进行 merge
        		__counter[__i].merge(__carry);
        		// 交换，看下一个位置是否也如此
        		__carry.swap(__counter[__i++]);
      		}
      		// 一轮归并、排序完毕，放回 counter 何时的位置
      		__carry.swap(__counter[__i]);         
      		if (__i == __fill) ++__fill;
    	}
    	// 将 counter 中所有元素 merge 后，得到最终排序列表
    	for (int __i = 1; __i < __fill; ++__i)
      		__counter[__i].merge(__counter[__i-1]);
    	swap(__counter[__fill-1]);
  	}
}
```

初看下，有点难以理解，其实这是归并排序的 Bottom-Up 迭代实现。具体代码解析可以参照博客：[归并排序的迭代写法](https://sosohu.github.io/algorithm/2014/04/14/%E5%BD%92%E5%B9%B6%E6%8E%92%E5%BA%8F%E7%9A%84%E8%BF%AD%E4%BB%A3%E5%86%99%E6%B3%95.html) 和 [STL库list::sort()实现深度解析](http://www.cnblogs.com/avota/p/5388865.html)

话又说回来，能不能用 Top-Down 递归来写链表的排序呢？

实际上`clang`的`libc++`就是这样做的，代码在 `include/list.h - list<_Tp, _Alloc>::__sort(iterator __f1, iterator __e2, size_type __n, _Comp& __comp)`，限于篇幅，这里就不摘出来了。