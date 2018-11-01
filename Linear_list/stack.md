# Stack

Stack（栈）是一种操作受限的抽象数据类型。操作受限是指 Stack 只能在线性结构的一段（称为堆栈顶端）加入或删除数据。这种操作特性可以用 `LIFO`（Last In First Out，后进先出）来总结。

## Stack in C++

[std::stack](https://zh.cppreference.com/w/cpp/container/stack)是 STL 提供的一种容器适配器，给与用户栈的抽象。说它是容器适配器，是因为 

```c++
template<calss T, class Container=std::deque<T>>
class stack;
```
这个模板其实是底层容器（默认为 `std::deque`）的包装器，只提供特定函数集合。

```c++
std::stack<int> foo;
foo.push(1);
foo.push(2);
cout << foo.top() << endl;
foo.pop();
```

[std::deque](https://zh.cppreference.com/w/cpp/container/deque)（double-ended queue)是一种双端队列，允许在首尾两端快速插入删除元素。

```
std::deque<int> foo = {2, 3}
foo.push_front(1);
foo.push_back(4);
```
我们稍后再来看 `std::deque` 的实现，先把目光放在 `std::stack` 是如何包装其底层容器的。

```c++
template <class _Tp, class _Sequence>
class stack {

	// stack 不提供 iterator
	// 实际包装的底层容器，默认为 deque，也可以指定为 vector / list
	_Sequence c;
	
	// 提供的受限操作，都是直接使用底层容器的操作
	size_type size() const { return c.size(); }
	reference top() { return c.back(); }
	void push(const value_type& __x) { c.push_back(__x); }
	void pop() { c.pop_back(); }
};
```

可以看到，`std::stack` 的关键在于其底层容器，其余的操作都是包装底层容器的操作。也就是说，只要底层容器提供 `push_back` 和 `pop_back()` 操作，就可以作为 `std::stack` 的底层容器。除了默认的 `deque` 外，`vector` 和 `list` 都满足这些要求。使用`list`作为底层容器的栈就是链式栈。

`std::list` 的结构已经在 [Linked List](https://github.com/yichenluan/Algorithm101/blob/master/Linear_list/linked_list.md) 中解析过了，下面让我们来研究 `std::deque` 的设计：

```c++
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class deque : protected _Deque_base<_Tp, _Alloc> {

	typedef _Deque_base<_Tp, _Alloc> _Base;
	typedef typename _Base::iterator       iterator;
};

explicit deque(const allocator_type& __a = allocator_type()) 
	: _Base(__a, 0) {}

template <class _Tp, class _Alloc>
class _Deque_base {
	// 迭代器类型
	typedef _Deque_iterator<_Tp,_Tp&,_Tp*>             iterator;
	// 构造函数
  	_Deque_base(const allocator_type&, size_t __num_elements)
    	: _M_map(0), _M_map_size(0),  _M_start(), _M_finish() {
    	_M_initialize_map(__num_elements);
  	}
  	
  	// 数据成员
  	// _M_map 保存的是 _Tp* 的指针数组
  	_Tp** _M_map;
  	size_t _M_map_size;  
  	// 指向最首端迭代器
  	iterator _M_start;
  	// 指向最尾端迭代器
  	iterator _M_finish;
};

template <class _Tp, class _Ref, class _Ptr>
struct _Deque_iterator {
	// iterator_traits 所需的属性
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp** _Map_pointer;
	
	// iterator 包含的成员
	// 指向数据段中的当前元素
	_Tp* _M_cur;
	// 指向数据段的头部
	_Tp* _M_first;
	// 指向数据段的尾部
	_Tp* _M_last;
	// 指向该数据段对应在 _M_map 的 node
	_Map_pointer _M_node;
};
```

为了便于理解，引用 [steveLauwh/SGI-STL](https://github.com/steveLauwh/SGI-STL) 所画的图：

![](https://github.com/steveLauwh/SGI-STL/raw/master/The%20Annotated%20STL%20Sources%20V3.3/Other/deque%20DS.PNG)

那么构建就在于两点：

- deque 如何组织数据
- deque 如何模拟连续空间

首先来看组织数据的关键代码：

```c++
// 确定缓冲区容纳元素个数的函数
// 元素 size 小于 512，返回 512/size
// 否则返回 1，即每个缓冲区只容纳一个元素
inline size_t __deque_buf_size(size_t __size) {
  return __size < 512 ? size_t(512 / __size) : size_t(1);
}

template <class _Tp, class _Alloc>
void _Deque_base<_Tp,_Alloc>::_M_initialize_map(size_t __num_elements) {
	// 确定 _M_map 的初始大小，并分配空间
	// 初始 __num_elements 为 0，分配 _S_initial_map_size，即 8 个节点
	size_t __num_nodes = __num_elements / __deque_buf_size(sizeof(_Tp)) + 1;
	_M_map_size = max((size_t) _S_initial_map_size, __num_nodes + 2);
	_M_map = _M_allocate_map(_M_map_size);
	
	// 这里比较巧妙，这一步是将有数据的 node 分配到 map 中间
	// 然后，将 start, finish 指向有数据的 node 的两端
	_Tp** __nstart = _M_map + (_M_map_size - __num_nodes) / 2;
	_Tp** __nfinish = __nstart + __num_nodes;
	
	// 为每个有数据的节点分配实际缓冲区空间
	_M_create_nodes(__nstart, __nfinish);
	
	// 调整迭代器，使得 _M_start，_M_finish 指向两端的节点，且对应的 _M_cur 指向元素两端。
	_M_start._M_set_node(__nstart);
	_M_finish._M_set_node(__nfinish - 1);
	_M_start._M_cur = _M_start._M_first;
	_M_finish._M_cur = _M_finish._M_first + __num_elements % __deque_buf_size(sizeof(_Tp));
}

// 以最典型的 insert 操作示例
iterator deque::insert(iterator position, const value_type& __x) {
	if (position._M_cur == _M_start._M_cur) {
		// 如果 position 是最头部，则 push_front
		push_front(__x);
		return _M_start;
	} else if (position._M_cur == _M_finish._M_cur) {
		// 如果 position 是最尾部，则 push_back
		push_back(__x);
		iterator __tmp = _M_finish;
		--__tmp;
		return __tmp;
	} else {
		// 一定要移动元素了，只好 insert_aux
		return _M_insert_aux(position, __x);
	}
}

void push_front() {
	if (_M_start._M_cur != _M_start._M_first) {
		// 如果当前缓冲区头部还有空间
		construct(_M_start._M_cur - 1);
      	--_M_start._M_cur;
	} else
		// 需要分配空间
		_M_push_front_aux();
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_push_front_aux()
{
  	_M_reserve_map_at_front();
  	// 分配新的 node
  	*(_M_start._M_node - 1) = _M_allocate_node();
  	// 调整 _M_start 的指向
  	_M_start._M_set_node(_M_start._M_node - 1);
  	_M_start._M_cur = _M_start._M_last - 1;
  	// 在新 node 的最后一个位置放入元素
  	construct(_M_start._M_cur);
}

void _M_reserve_map_at_front (size_type __nodes_to_add = 1) {
	if (__nodes_to_add > size_type(_M_start._M_node - _M_map))
		// _M_map 没有足够的 node 了，需要重新扩展 map
		_M_reallocate_map(__nodes_to_add, true);
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_reallocate_map(size_type __nodes_to_add,
                                          bool __add_at_front)
{
	size_type __old_num_nodes = _M_finish._M_node - _M_start._M_node + 1;
	size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;
	_Map_pointer __new_nstart;
	// ...
	// 两倍扩张
	size_type __new_map_size = _M_map_size + max(_M_map_size, __nodes_to_add) + 2;
	// 重新分配新的 _M_map
	_Map_pointer __new_map = _M_allocate_map(__new_map_size);
	// 确定 start 的位置，这个位置是从中间向外平摊
	__new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2 
					+ (__add_at_front ? __nodes_to_add : 0);
	// 复制指针
	copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
	// 析构原有 map
	_M_deallocate_map(_M_map, _M_map_size);
	// 调整数据成员
	_M_map = __new_map;
	_M_map_size = __new_map_size;
	_M_start._M_set_node(__new_nstart);
	_M_finish._M_set_node(__new_nstart + __old_num_nodes - 1);
}

// 限于篇幅，_M_insert_aux 就不摘录出来了，核心思想与前面保持一致，如果插入点前面的元素较少，则移动前面的元素来腾出位置
// 否则，移动后面的元素。
```

现在我们知道了,`deque`是分段连续的，那么，`deque` 是如何对外模拟连续空间的呢？答案就在迭代器重载上：

```c++
// 注意，这里的 + 号是被操作符重载了
reference operator[](difference_type __n) const { return *(*this + __n); }

// 迭代器移动 n
_Self& operator+=(difference_type __n) {
	// 移动后的位置在缓存段的位置
	// 这里的 - 也是操作符重载了
	difference_type __offset = __n + (_M_cur - _M_first);
	if (__offset >= 0 && __offset < difference_type(_S_buffer_size())) {
		// 在同一缓冲区内
		_M_cur += __n;
	} else {
		// 计算属于目标缓冲区的 offset
		difference_type __node_offset =
						__offset > 0 ? __offset / difference_type(_S_buffer_size())
						: -difference_type((-__offset - 1) / _S_buffer_size()) - 1;
		 // 移动到正确的缓冲区
		 _M_set_node(_M_node + __node_offset);
		 // 移动到正确的元素
		 _M_cur = _M_first +
		 		(__offset - __node_offset * difference_type(_S_buffer_size()));
	}
	return *this;
}
```

## Stack in Go

Go 中没有单独为 `Stack` 提供容器。不过我们可以自己通过 `slice` 和 `container/list` 来实现顺序栈和链式栈。

```go
// 暂略
```