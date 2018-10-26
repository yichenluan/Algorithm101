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

与 C++ 不同，Go 在函数调用时，传递数组并不会隐式的将数组作为指针，而是，传递这个数组的一份 Copy。

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