# Queue

Queue（队列）是一种操作受限的抽象数据类型。Queue 只能在线性结构的某一端加入元素，只能在另一端取出数据。这种操作特性可以用 `FIFO`（First In First Out）来总结。

## Queue in C++

类似于[std::stack](https://zh.cppreference.com/w/cpp/container/stack)，[std::queue](https://zh.cppreference.com/w/cpp/container/queue)也是 STL 中的一种容器适配器，具体来说，Queue 只提供特定函数的集合，从底层容器尾端推入元素、从首端弹出元素。

```c++
template <class _Tp, class _Sequence>
class queue;
```
Queue 包装底层容器（默认为 `std:deque`），提供有限的操作：

```c++
std::queue<int> foo;
foo.push(1);
foo.push(2);
cout << foo.front() << endl;
foo.pop();
```

`std::queue` 的实现非常简单：

```c++
template <class _Tp, class _Sequence>
class queue {
	_Sequence c;  // 底层容器
	
	reference front() { return c.front(); }
	void push(const value_type& __x) { c.push_back(__x); }
	void pop() { c.pop_front(); }
};
```

`std::queue` 要求其底层容器实现 `back()`、`front()`、`push_back()`、`pop_front()`操作，STL 中的标准容器除 `std::deque` 外，`std::list` 也满足这些要求。

## Queue in Go

Go 标准库没有单独提供 `Queue` 容器，不过 `Channel` 的实现中包含了一个环形队列，代码解析可参照我之前的博客：[Go in Depth - Channel 源码解析](http://jinke.me/2018-05-04-channel-source/)。

## Concurrent Queue

TODO