# SkipList

跳表（SkipList）是由 William Pugh 在论文 [Skip Lists: A Probabilistic Alternative to
Balanced Trees](Skip Lists: A Probabilistic Alternative to
Balanced Trees) 中提出的一种数据结构。

> Skip lists are a data structure that can be used in place of balanced trees.

跳表使用概率均衡技术，在原始链表的基础上，建立多级索引，加快链表的插入、查找、删除等操作。保证绝大多数操作均拥有 O(logn) 的时间复杂度，这也就意味着，可以使用这种数据结构来代替实现复杂的平滑查找树（如红黑树）。

![](https://sleepy-1256633542.cos.ap-beijing.myqcloud.com/20181210145134.png)

如上图所示，`a` 到 `d` 中，对每一层而言，隔两个链表节点，即在上层新增一个额外的指针作为索引。经过数学计算，这样的结构查询效率为 O(logn)。不过如果严格按照这种规则进行新增删除，会使得操作非常复杂。

我们称一个拥有 `k` 个指针的结点为 `k` 层结点，那么，从概率的角度上看，`50%` 的结点为 `1` 层结点，`25%` 的结点为 `2` 层结点。那么，我们只要保证每层结点的分布保持上述概率，则仍然可以得到相同的操作复杂度。即图 `e` 所示结构。

![](https://sleepy-1256633542.cos.ap-beijing.myqcloud.com/20181210145848.png)

通过跳表进行查找的过程如上图所示。利用高层的索引指针快速缩小遍历范围，层层递进，直到找到最低层结点为止。

使用 SkipList 作为核心数据结构的有 `LevelDB` 和 `Redis` 等，其实现大同小异，下面我们来解析 `LevelDB` 的实现。

## SkipList in LeveDB

`LevelDB` 使用 `SkipList` 来实现内存数据库，维护有序的 `Key-Value Pair`。结构如下：

```c++
// SkipList
template<typename Key, class Comparator>
class SkipList {
private:
	struct Node;
	
public:
	explicit SkipList(Comparator cmp, Arena* arena);
	void Insert(const Key& key);
	bool Contains(const Key& key) const;
	
private:
	enum { kMaxHeight = 12 };
	// Comparator 模板参数
	Comparator const compare_;
	// 用以分配内存 (alloctor)
	Arena* const arena_;
	// Head 指针
	Node* const head_;
	// 当前最高层数
	port::AtomicPointer max_height_;
	// 随机种子
	Random rnd_;
	// 新建指定高度的结点
	Node* NewNode(const Key& key, int height);
	int RandomHeight();
	// 搜索过程
	Node* FindGreaterOrEqual(const Key& key, Node** prev) const;
};
```

```c++
// Node
template<typename Key, class Comparator>
struct SkipList<Key,Comparator>::Node {
	explicit Node(const Key& k) : key(k) { }
	Key const key;
	// 获取当前结点指定层数的 Next 结点
	Node* Next(int n) {
		return reinterpret_cast<Node*>(next_[n].Acquire_Load());
	}
	// 在指定层数放入 Next 结点
	void SetNext(int n, Node* x) {
		next_[n].Release_Store(x);
	}
	// 层级 Next 指针
	port::AtomicPointer next_[1];
};
```

构造一个 `SkipList` 的过程就是初始化 `head_` 节点。

```c++
template<typename Key, class Comparator>
SkipList<Key,Comparator>::SkipList(Comparator cmp, Arena* arena)
	: compare_(cmp)
	, arena_(arena)
	, head_(NewNode(0 /* any key will do */, kMaxHeight))
	, max_height_(reinterpret_cast<void*>(1))
	, rnd_(0xdeadbeef) {
	for (int i = 0; i < kMaxHeight; i++) {
		head_->SetNext(i, nullptr);
	}
}
```

`Insert` 分为三步：

1. 搜索正确的放置位置；
2. 确定随机高度，新建结点；
3. 根据第 1 步中保存的各层节点，插入链表节点。

```c++
template<typename Key, class Comparator>
void SkipList<Key,Comparator>::Insert(const Key& key) {
	Node* prev[kMaxHeight];
	// 找到位置，并填充 prev
	Node* x = FindGreaterOrEqual(key, prev);
	
	int height = RandomHeight();
	if (height > GetMaxHeight()) {
		// 如果随机高度大于现有高度，则需要先填充 prev 为 head
		for (int i = GetMaxHeight(); i < height; i++) {
			prev[i] = head_;
		}
		max_height_.NoBarrier_Store(reinterpret_cast<void*>(height));
	}
	// 新建拥有指定层数的节点
	x = NewNode(key, height);
	for (int i = 0; i < height; i++) {
		// 利用 prev，在每层插入该节点
		x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
		prev[i]->SetNext(i, x);
	}
}
```

搜索节点的过程如下:

```c++
template<typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::FindGreaterOrEqual(const Key& key, Node** prev) {
	Node* x = head_;
	// 从当前最高层找起
	int level = GetMaxHeight() - 1;
	while (true) {
		Node* next = x->Next(level);
		if (KeyIsAfterNode(key, next)) {
			// 在该层继续往前找
			x = next;
		} else {
			// 填充 prev
			if (prev != nullptr) prev[level] = x;
			if (level == 0) {
				// 如果是最后一层，则找到了直接返回
				return next;
			} else {
				// 否则，接入下一层去找
				level--;
			}
		}
	}
}
```

`LevelDB` 使用的概率为 `1/4`：

```c++
template<typename Key, class Comparator>
int SkipList<Key,Comparator>::RandomHeight() {
	static const unsigned int kBranching = 4;
	int height = 1;
	while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
		height++;
	}
	return height;
}
```