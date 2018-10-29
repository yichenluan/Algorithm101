# Array

数组(Array) 是一种线性表数据结构，通过一段连续的存储区域(内存空间)来存储元素。这句话的关键在于`连续`，因为连续的特性，计算机能够通过数组首地址以及数组下标计算得到偏移量，从而直接访问对应位置的数组元素，这个过程，也就是所谓的`随机访问`。

## Array in C++

以 C 的风格声明数组：

```c++
int foo[3] = {1, 2, 3};
```

通过 C++ 容器 `Array` ([cppreference](https://zh.cppreference.com/w/cpp/container/array))使用数组：

```c++
std::array<int, 3> foo = {1, 2,3 };
```

`std::array` 是对 c 风格数组的一种封装，允许 C++ 用户像其他容器一样使用 array，主要代码如下：

```c++
template <class _Tp, size_t _Size>
struct array {
	typedef _Tp                                   value_type;
	typedef value_type&                           reference;
	// iterator 就是 native pointer
	typedef value_type*                           iterator;
	
	// 唯一的数据成员
	_Tp __elems_[_Size];
};
```

我们拿出一些成员函数来观察 `array` 的设计：

```c++
value_type* data() _NOEXCEPT {return __elems_;}

iterator begin() _NOEXCEPT {return iterator(data());}

iterator end() _NOEXCEPT {return iterator(data() + _Size);}
```

## Array in Go

```go
// foo := [...]int{1, 2, 3}
var foo [3]int = [...]int{1, 2, 3}
```

与 C++ 不同，Go 在函数调用时，传递数组并不会隐式的将数组作为指针，而是，传递这个数组的一份 Copy，也就是说 `array` 在 Go 中是值类型(value)。

## Vector in C++

由于`array`限定了大小，且无法动态更改，我们在 C++ 中实际上很少使用`array`，`vector`([cppreference](https://zh.cppreference.com/w/cpp/container/vector)) 是 STL 提供的封装动态数组的顺序容器。

```c++
std::vector<int> foo = {1, 2};
foo.push_back(3);
```
我们来看下 Clang 对`vector`的实现：

```c++
template <class _Tp, class _Allocator>
class __vector_base {
    typedef _Allocator                               allocator_type;
    typedef allocator_traits<allocator_type>         __alloc_traits;
    typedef typename __alloc_traits::pointer         pointer;
    typedef pointer                                  iterator;
    // ...
    pointer                                         __begin_;
    pointer                                         __end_;
    __compressed_pair<pointer, allocator_type> __end_cap_;
};

template <class _Tp, class _Allocator /* = allocator<_Tp> */>
class _LIBCPP_TEMPLATE_VIS vector 
	: private __vector_base<_Tp, _Allocator>
{
	// 这个 point 绕了好大一圈，实际上就是 _Tp*
	// __wrap_iter 是一个迭代器的包裹，为什么要这个 wrapper，现在还不知道
	typedef __wrap_iter<pointer>                     iterator;
};
```
`vector` 区别于 `array` 最重要的就是动态扩容，我们摘出`push_back`的代码来一窥究竟：

```c++
size_type size() const _NOEXCEPT
	{return static_cast<size_type>(this->__end_ - this->__begin_);}
	
template <class _Tp, class _Allocator>
void
vector<_Tp, _Allocator>::push_back(const_reference __x) {
	// 不懂为啥全加上了 this
    if (this->__end_ != this->__end_cap())  // 有可用空间
    {
        __alloc_traits::construct(this->__alloc(),
                                  _VSTD::__to_raw_pointer(this->__end_), __x);
        ++this->__end_; // 调整 __end_ 迭代器
    }
    else
        __push_back_slow_path(__x);
}

template <class _Tp, class _Allocator>
template <class _Up>
vector<_Tp, _Allocator>::__push_back_slow_path(_Up&& __x) {
    allocator_type& __a = this->__alloc();
    // 分配新的空间，重点看 __recommend(..) 就好了
    __split_buffer<value_type, allocator_type&> __v(__recommend(size() + 1), size(), __a);
    __alloc_traits::construct(__a, _VSTD::__to_raw_pointer(__v.__end_), _VSTD::forward<_Up>(__x));
    __v.__end_++;
    // 重新给迭代器赋值
    __swap_out_circular_buffer(__v);
}

vector<_Tp, _Allocator>::__recommend(size_type __new_size) const
{
    const size_type __ms = max_size();
    if (__new_size > __ms)
        this->__throw_length_error();
    const size_type __cap = capacity();
    if (__cap >= __ms / 2)
        return __ms;
    // 返回 2 * __cap
    return _VSTD::max<size_type>(2*__cap, __new_size);
}

vector<_Tp, _Allocator>::__swap_out_circular_buffer(__split_buffer<value_type, allocator_type&>& __v)
{
    // 释放原有空间
    __alloc_traits::__construct_backward(this->__alloc(), this->__begin_, this->__end_, __v.__begin_);
    // 调整迭代器
    _VSTD::swap(this->__begin_, __v.__begin_);
    _VSTD::swap(this->__end_, __v.__end_);
    _VSTD::swap(this->__end_cap(), __v.__end_cap());
    __v.__first_ = __v.__begin_;
    __invalidate_all_iterators();
}
```

emm，代码非常复杂，看上去也有很多的模板技巧，但是现在我们只关心`vector`的逻辑实现就够了。

## Slice in Go

`Slice` 是基于数组的一种数据类型：

```
foo := make([]int, 3, 10)
```

[Go Slices: usage and internals](https://blog.golang.org/go-slices-usage-and-internals) 详细描述了 Slice 的方方面面。

![](http://p890o7lc8.bkt.clouddn.com/20181027125317.png)

下面让我们深入源码观察 `Slice`：

```go
// 结构
type slice struct {
	array unsafe.Pointer
	len   int
	cap   int
}
```

```go
// 创建 slice
func makeslice(et *_type, len, cap int) slice {
	// 根据切片的数据类型，获取切片的最大容量
	maxElements := maxSliceCap(et.size)
    // 比较切片的长度，长度值域应该在[0,maxElements]之间
	if len < 0 || uintptr(len) > maxElements {
		panic(errorString("makeslice: len out of range"))
	}
    // 比较切片的容量，容量值域应该在[len,maxElements]之间
	if cap < len || uintptr(cap) > maxElements {
		panic(errorString("makeslice: cap out of range"))
	}
    // 根据切片的容量申请内存
	p := mallocgc(et.size*uintptr(cap), et, true)
    // 返回申请好内存的切片的首地址
	return slice{p, len, cap}
}
```

```go
// 动态扩容
// expand append(l1, l2...) to
//   init {
//     s := l1
//     if n := len(l1) + len(l2) - cap(s); n > 0 {
//       s = growslice_n(s, n)
//     }
//     s = s[:len(l1)+len(l2)]
//     memmove(&s[len(l1)], &l2[0], len(l2)*sizeof(T))
//   }
//   s
//
// l2 is allowed to be a string.
func growslice(et *_type, old slice, cap int) slice {
	// ...
	
	// 1. 扩容策略
	newcap := old.cap
	doublecap := newcap + newcap
	if cap > doublecap {
		// 如果需要的大小大于 doublecap，则新 cap 为指定的 cap
		newcap = cap
	} else {
		if old.len < 1024 {
			// 如果 old.len 小于 1024，则新 cap 为 doublecap
			newcap = doublecap
		} else {
			// Check 0 < newcap to detect overflow
			// and prevent an infinite loop.
			for 0 < newcap && newcap < cap {
				// 如果 old.len 大于 1024，则 old.cap 以 1.25 倍增大，直到大于指定的 cap
				newcap += newcap / 4
			}
			// Set newcap to the requested cap when
			// the newcap calculation overflowed.
			if newcap <= 0 {
				newcap = cap
			}
		}
	}

	// 2. 计算实际需要分配的空间大小
	var overflow bool
	var lenmem, newlenmem, capmem uintptr
	// Specialize for common values of et.size.
	// For 1 we don't need any division/multiplication.
	// For sys.PtrSize, compiler will optimize division/multiplication into a shift by a constant.
	// For powers of 2, use a variable shift.
	switch {
	case et.size == 1:
		// 旧 slice len 所需要的空间大小
		lenmem = uintptr(old.len)
		// 新 slice len 所需要的空间大小
		newlenmem = uintptr(cap)
		// 新 slice cap 所需要的空间大小
		capmem = roundupsize(uintptr(newcap))
		overflow = uintptr(newcap) > maxAlloc
		newcap = int(capmem)
	//...
	default:
		lenmem = uintptr(old.len) * et.size
		newlenmem = uintptr(cap) * et.size
		capmem, overflow = math.MulUintptr(et.size, uintptr(newcap))
		capmem = roundupsize(capmem)
		newcap = int(capmem / et.size)
	}
	
	// ...

	// 3. 分配内存空间，完成 copy 和初始化工作
	var p unsafe.Pointer
	if et.kind&kindNoPointers != 0 {
		// 非指针类型，mallocgc 时不需要清零
		p = mallocgc(capmem, nil, false)
		// The append() that calls growslice is going to overwrite from old.len to cap (which will be the new length).
		// Only clear the part that will not be overwritten.
		// 将 [newlen:cap] 这一段赋零值
		memclrNoHeapPointers(add(p, newlenmem), capmem-newlenmem)
	} else {
		// Note: can't use rawmem (which avoids zeroing of memory), because then GC can scan uninitialized memory.
		p = mallocgc(capmem, et, true)
		// 指针类型，分配的空间已经清零
		if writeBarrier.enabled {
			// Only shade the pointers in old.array since we know the destination slice p
			// only contains nil pointers because it has been cleared during alloc.
			// 不太懂这个是干嘛的
			bulkBarrierPreWriteSrcOnly(uintptr(p), uintptr(old.array), lenmem)
		}
	}
	// 将之前的元素 copy 过来
	memmove(p, old.array, lenmem)

	return slice{p, old.len, newcap}
}
```

这里面需要注意的是，我看很多博客说如果 slice 底下的 array 还有空间，分配空间时会优先使用现有空间，但是我在代码中没看到相关逻辑，也没有在测试程序中复现这种现象。

```go
a := [...]int{1, 2, 3, 4, 5, 6, 7, 8} // L1
b := a[:1:1]   // L2
b = append(b, 0)  // L3

// a: [1, 2, 3, 4, 5, 6, 7, 8]
// b: [1, 0]
```

即，在 L3 中，b 实际是被分配到一块新的内存中了，并不会影响原有数组。