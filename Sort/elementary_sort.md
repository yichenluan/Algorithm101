# 初级排序算法

在详细描述各种排序算法之前，我们先列出分析评价一个排序算法的几大要素：

1. 执行效率（最好、最坏、平均时间复杂度）
2. 内存消耗（空间复杂度，为 O(1) 的算法称为原地排序算法）
3. 稳定性（大小相等的两个元素在排序后保持其相对顺序）

初级排序算法指的是平均时间复杂度为 O(n^2) 的算法，其中包括：冒泡排序、插入排序、选择排序、希尔排序。

## 冒泡排序（Bubble Sort）

[冒泡排序](https://zh.wikipedia.org/wiki/%E5%86%92%E6%B3%A1%E6%8E%92%E5%BA%8F)的思想就是每次遍历过程中，两两比较相邻元素，顺序与预期相反则进行交换，这样，完成一次遍历即可保证当前遍历范围内的最大（小)的元素已经浮（沉）到了合适的位置。

对冒泡排序的一个优化是：当某次遍历时发现没有交换任何元素，则代表数组已经有序，即可完成排序过程。

```c++
// C++
template<class RandomIt>
void bubble_sort(RandomIt first, RandomIt last) {
    if (first >= last) {
        return;
    }
    int round = 0;
    for (RandomIt i = first; i != last; ++i, ++round) {
        bool swapped = false;
        for (RandomIt curr = first; curr < last-round-1; ++curr) {
            if (*curr > *(curr+1)) {
                swap(*curr, *(curr+1));
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
    return;
}
```

```go
// Go
func BubbleSort(data sort.Interface) {
	n := data.Len()
	for i := 0; i < n; i++ {
		swapped := false
		for j := 0; j < n-i-1; j++ {
			if data.Less(j+1, j) {
				data.Swap(j, j+1)
				swapped = true
			}
		}
		if !swapped {
			break
		}
	}
	return
}
```

冒泡排序有一种变形优化，称之为[鸡尾酒排序](https://zh.wikipedia.org/wiki/%E9%B8%A1%E5%B0%BE%E9%85%92%E6%8E%92%E5%BA%8F)，优化点在于从左到右完成一次冒泡后，继而从右到左完成一次反向冒泡。对于已经基本有序的数组而言，效率会稍高些，不过本质没什么区别，这里就不列出来了。

分析下冒泡排序的三大要素：

1. 时间复杂度

	- 最好情况 O(n)：数组已经有序，只需要一次遍历。
	- 最坏情况 O(n^2)：数组倒序，需要 n 次冒泡。
	- 平均情况 O(n^2)：可以根据有序度来分析。

2. 空间复杂度

	冒泡排序是原地排序算法。
	
3. 稳定性

	我们的代码中当相邻元素相等时，不做交换，所以冒泡排序是稳定排序算法。
	
## 插入排序（Insertion Sort)

[插入排序](https://zh.wikipedia.org/wiki/%E6%8F%92%E5%85%A5%E6%8E%92%E5%BA%8F) 将数组视为已排序部分和未排序部分，排序过程就是挨个将未排序部分的元素插入到已排序部分的合理位置。当未排序部分为空时，排序完成。

```c++
// C++
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
```

```go
// Go
func InsertionSort(data sort.Interface) {
	n := data.Len()
	for i := 1; i < n; i++ {
		for j := i; j > 0 && data.Less(j, j-1); j-- {
			data.Swap(j, j-1)
		}
	}
}
```

插入排序的一种优化是通过二分查找来减少在已排序部分的比较次数，称之为[二分查找插入排序](https://www.geeksforgeeks.org/binary-insertion-sort/)。

1. 时间复杂度

	- 最好情况 O(n)：数据已经有序
	- 最坏情况 O(n^2)：数据倒序
	- 平均情况 O(n^2)

2. 空间复杂度

	插入排序是原地排序算法。

3. 稳定性

	类似于冒泡排序，对于值相同的元素，后面的元素继续放在之前出现的元素的后面即可。所以，插入排序是稳定排序算法。
	
虽然看上去插入排序和冒泡排序时间复杂度一样，但是一般来说插入排序的效率要比冒泡快上一些，原因在于冒泡排序每次交换都需要 3 次赋值操作（swap），而插入排序只需要一次赋值操作。
	
## 选择排序（Selection Sort）

[选择排序](https://zh.wikipedia.org/wiki/%E9%80%89%E6%8B%A9%E6%8E%92%E5%BA%8F)是最直观的一种排序算法，每次遍历数组的全部未排序部分，找到最小的放在已排序部分的末端，以此类推，直到排序完毕。

```c++
// C++
template<class RandomIt>
void selection_sort(RandomIt first, RandomIt last) {
    if (first >= last) {
        return;
    }
    for (RandomIt i = first; i != last-1; ++i) {
        RandomIt min_it = i;
        for (RandomIt j = i; j != last; ++j) {
            min_it = (*j < * min_it)? j: min_it;
        }
        swap(*i, * min_it);
    }
    return;
}
```

```go
// Go
func SelectionSort(data sort.Interface) {
	n := data.Len()
	for i := 0; i < n-1; i++ {
		minIdx := i
		for j := i; j < n; j++ {
			if data.Less(j, minIdx) {
				minIdx = j
			}
		}
		data.Swap(minIdx, i)
	}
}
```

1. 时间复杂度
	- 最好情况 O(n^2)
	- 最坏情况 O(n^2)
	- 平均情况 O(n^2)

2. 空间复杂度

	O(1)，原地排序算法
	
3. 稳定性

	选择排序是一种不稳定算法，因为在遍历过程中，会将元素跨越性的进行交换。
	
## 希尔排序（Shell Sort）

TODO
