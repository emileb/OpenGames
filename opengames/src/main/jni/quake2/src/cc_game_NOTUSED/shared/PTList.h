// TList.h
// my interpretation of .NET's lists
#include <map>

template <typename T>
class TLinkedList
{
public:
	class Node
	{
	public:
		T	Value;

		Node(const T &value)
		{
			Value = value;
			Next = Prev = null;
		}

		Node	*Next,
				*Prev;
	};

	uint32		_count; // makes counting faster if I do it manually
	Node		*_head,
				*_tail;

	TLinkedList ()
	{
		_head = _tail = null;
		_count = 0;
	}

	Node *Head() const
	{
		return _head;
	}

	Node *Tail() const
	{
		return _tail;
	}

	void Add (const T &value)
	{
		AddToBack(value);
	}

	Node *AddToBack (const T &value)
	{
		Node *node = new Node(value);

		if (_head == null)
		{
			_head = _tail = node;
			_count++;
			return node;
		}

		_tail->Next = node;
		node->Prev = _tail;
		_tail = node;
		_count++;

		return node;
	}

	Node *AddToFront (const T &value)
	{
		if (_head == null)
			return AddToBack(value);

		Node *node = new Node(value);

		node->Next = _head;
		_head->Prev = node;
		_head = node;
		_count++;

		return node;
	}

	// NOTE: node will be invalidated after this call.
	void RemoveNode (Node *node)
	{
		if (node->Prev != null)
			node->Prev->Next = node->Next;
		else
			_head = node->Next;

		if (node->Next != null)
			node->Next->Prev = node->Prev;
		else
			_tail = node->Prev;

		_count--;
		delete node;
	}
	
	void RemoveFromFront ()
	{
		RemoveNode(_head);
	}

	void RemoveFromBack()
	{
		RemoveNode(_tail);
	}

	T PopFront ()
	{
		T value = _head->Value;
		RemoveFromFront();
		return value;
	}

	T PopBack ()
	{
		T value = _tail->Value;
		RemoveFromBack();
		return value;
	}

	void InsertAt(const T &value, const uint32 index)
	{
		throw Exceptions::NotImplemented();
	}

	void InsertAt(const T &value, Node *node)
	{
		throw Exceptions::NotImplemented();
	}

	void AddRange (const T *values, const uint32 numValues)
	{
		throw Exceptions::NotImplemented();
	}

	uint32 Count() const
	{
		return _count;
	}

	uint32 Capacity() const
	{
		return _count;
	}

	void RemoveAt (const uint32 index)
	{
		throw Exceptions::NotImplemented();
	}

	void Remove (const T &value)
	{
		for (Node *node = _head; node != null; node = node->Next)
		{
			if (node->Value == value)
			{
				RemoveNode(node);
				break;
			}
		}
	}

	void Clear()
	{
		while (_head != null)
			RemoveNode(_head);
	}
	
	T &ValueAt (const uint32 index) const
	{
		uint32 i = 0;

		for (Node *node = _head; node != null; node = node->Next)
		{
			if (i == index)
				return node->Value;

			++i;
		}

		throw Exception();
	}
	
	Node *NodeAt (const uint32 index) const
	{
		uint32 i = 0;

		for (Node *node = _head; node != null; node = node->Next)
		{
			if (i == index)
				return node;

			++i;
		}

		return null;
	}
	
	int IndexOf (const T &value) const
	{
		uint32 i = 0;

		for (Node *node = _head; node != null; node = node->Next)
		{
			if (node->Value == value)
				return i;

			++i;
		}

		return -1;
	}
};

template <typename T>
class TStack
{
protected:
	TLinkedList<T> _list;

public:
	TStack()
	{
	}

	void Push(const T &value)
	{
		_list.AddToBack(value);
	}

	T Pop()
	{
		T end = _list.Tail()->Value;
		_list.RemoveFromBack();
		return end;
	}

	T Peek()
	{
		return _list.Tail()->Value;
	}

	int Count()
	{
		return _list.Count();
	}

	void Clear()
	{
		_list.Clear();
	}
};

template <typename T>
class TQueue
{
protected:
	TLinkedList<T> _list;

public:
	TQueue()
	{
	}

	void Push(const T &value)
	{
		_list.AddToBack(value);
	}

	T Pop()
	{
		T end = _list.Head()->Value;
		_list.RemoveFromFront();
		return end;
	}

	T Peek()
	{
		return _list.Head()->Value;
	}

	int Count()
	{
		return _list.Count();
	}

	void Clear()
	{
		_list.Clear();
	}
};

template <typename T>
class TList
{
protected:
	T *_array;
	uint32 _reserved, _count;
	uint32 _granularity;
	bool _resetReserved;

	typedef TypeInfo<T> TTypeInfo;

	void ConstructFromValues (const T *values, uint32 numValues)
	{
		_reserved = numValues + _granularity;
		_count = numValues;
		_array = new T[_reserved];

		if (TTypeInfo::NeedsCtor)
		{
			for (uint32 i = 0; i < numValues; ++i)
				_array[i] = values[i];
		}
		else
			memcpy(_array, values, TTypeInfo::Size * numValues);
	}
	
	void ConstructFromReserved (uint32 reserved)
	{
		_reserved = _granularity;
		_count = 0;
		_array = new T[reserved];
	}

	void Resize (uint32 newSize)
	{
		T *oldArray = _array;
		uint32 oldCount = _count;

		_reserved = newSize + _granularity;

		_array = new T[_reserved];

		if (TTypeInfo::NeedsCtor)
		{
			for (uint32 i = 0; i < oldCount; ++i)
				_array[i] = oldArray[i];
		}
		else
			memcpy(_array, oldArray, TTypeInfo::Size * oldCount);
		
		delete[] oldArray;
	}

	void Destroy ()
	{
		delete[] _array;
	}

public:
	TList (const T *values, const uint32 numValues, const uint32 granularity = 4)		
	{
		_granularity = granularity;
		ConstructFromValues(values, numValues);
		_resetReserved = true;
	}

	TList (const TList &copy, const uint32 granularity = 4)
	{
		_granularity = granularity;
		_reserved = copy.Capacity();
		_count = copy.Count();

		_array = new T[_reserved];
		
		if (TTypeInfo::NeedsCtor)
		{
			for (uint32 i = 0; i < _reserved; ++i)
				_array[i] = copy._array[i];
		}
		else
			memcpy(_array, copy._array, TTypeInfo::Size * _reserved);

		_resetReserved = true;
	}

	TList (const uint32 granularity = 4)
	{
		_count = 0;
		_granularity = granularity;
		ConstructFromReserved(_granularity);
		_resetReserved = true;
	}

	~TList()
	{
		Destroy();
	}

	TList &operator = (const TList &r)
	{
		Destroy();

		_reserved = r.Capacity();
		_count = r.Count();

		_array = new T[_reserved];
		
		if (TTypeInfo::NeedsCtor)
		{
			for (uint32 i = 0; i < _count; ++i)
				_array[i] = r._array[i];
		}
		else
			memcpy(_array, r._array, TTypeInfo::Size * _count);

		return *this;
	}

	void Add (const T &value)
	{
		if (_count >= _reserved)
			Resize(_count + 1);

		_array[_count++] = value;
	}

	void AddRange (const T *values, const uint32 numValues)
	{
		// FIXME: quicker way with memcpy maybe?
		for (uint32 i = 0; i < numValues; ++i)
			Add(values[i]);
	}

	void AddRange (const TList &copy)
	{
		AddRange(copy._array, copy._count);
	}

	typedef int (*realComparerType) (const T &l, const T &r);

	static realComparerType &GetRealComparer()
	{
		static int (*realComparer) (const T &l, const T &r);
		return realComparer;
	}
	inline static int TempComparer (const void *l, const void *r)
	{
		return GetRealComparer()(*(T*)l, *(T*)r);
	}

	void Sort(int (*comparer) (const T &l, const T &r), int left, int right)
	{
		GetRealComparer() = comparer;
	
		std::qsort(_array, _count, TTypeInfo::Size, TempComparer);
	}

	void Sort (int (*comparer) (const T &l, const T &r))
	{
		Sort(comparer, 0, _count - 1);
	}

	uint32 Count() const
	{
		return _count;
	}

	uint32 Capacity() const
	{
		return _reserved;
	}

	const T *Array() const
	{
		return _array;
	}

	// FIXME: if (TTypeInfo::NeedsCtor)
	void RemoveAt (const uint32 index, const bool autoResize)
	{
		uint32 oldCount = _count;
		uint32 oldReserved = _reserved;
		
		_count--;

		bool reconstruct = (_reserved - _count) > _granularity;

		T *ptr = _array;

		if (reconstruct)
		{
			_reserved -= _granularity;
			ptr = new T[_reserved];
		}

		if (index == 0)
		{
			if (TTypeInfo::NeedsCtor)
			{
				for (uint32 i = 0; i < oldCount - 1; ++i)
					ptr[i] = _array[i + 1];
			}
			else
				memcpy(ptr, _array + 1, (oldCount - 1) * TTypeInfo::Size);
		}
		else if (index == oldCount - 1)
		{
			if (TTypeInfo::NeedsCtor)
			{
				for (uint32 i = 0; i < oldCount - 1; ++i)
					ptr[i] = _array[i];
			}
			else if (ptr != _array)
				memcpy(ptr, _array, (oldCount - 1) * TTypeInfo::Size);
		}
		else
		{
			// split array up
			T *oldLeft, *oldRight;
			uint32 oldLeftCount = 0, oldRightCount = 0;

			oldLeft = _array;
			oldLeftCount = index;

			oldRight = _array + index + 1;
			oldRightCount = oldCount - index - 1;

			uint32 in = 0;

			if (TTypeInfo::NeedsCtor)
			{
				for (; in < oldLeftCount; ++in)
					ptr[in] = oldLeft[in];

				for (in = 0; in < oldRightCount; ++in)
					ptr[oldLeftCount + in] = oldRight[in];
			}
			else
			{
				if (ptr != _array)
					memcpy(ptr, oldLeft, oldLeftCount * TTypeInfo::Size);

				memcpy(ptr + oldLeftCount, oldRight, oldRightCount * TTypeInfo::Size);
			}
		}

		if (reconstruct)
		{
			delete[] _array;
			_array = ptr;
		}
	}

	void RemoveAt (const uint32 index)
	{
		RemoveAt(index, (_resetReserved) ? true : false);
	}

	void Remove (const T &value, const bool autoResize)
	{
		RemoveAt(IndexOf(value), autoResize);
	}

	void Remove (const T &value)
	{
		RemoveAt(IndexOf(value));
	}

	void InsertAt (const T &value, const uint32 index)
	{
		if (_count == 0 || index == Count())
		{
			Add(value);
			return;
		}
		
		if (_count >= _reserved)
			Resize(_count + 1);

		//if (!TTypeInfo::NeedsCtor)
		//	memcpy(_array + index + 1, _array + index, _count - index);
		//else
		{
			for (int i = _count + 1; i > index; --i)
				_array[i] = _array[i - 1];
		}

		_count++;
		_array[index] = value;
	}

	void Clear ()
	{
		if (_resetReserved)
		{
			delete[] _array;
			ConstructFromReserved(_granularity);
		}
		else
			_count = 0;
	}

	inline T &ValueAt (const uint32 index) const
	{
		if (index < 0 || index >= _count)
			throw Exceptions::IndexOutOfRange("index");

		return _array[index];
	}

	int IndexOf (const T &value) const
	{
		for (uint32 i = 0; i < _count; ++i)
		{
			if (_array[i] == value)
				return i;
		}

		return -1;
	}

	bool Contains (const T &value) const
	{
		return IndexOf(value) != -1;
	}

	TList &operator+= (const T &value)
	{
		Add(value);
		return *this;
	}

	TList &operator-= (const T &value)
	{
		Remove(value);
		return *this;
	}

	inline T &operator[] (const uint32 index) const
	{
		return ValueAt(index);
	}
};

template<typename T, class TComparer = ComparingProvider<T>>
class TSortedListAdapter
{
	typedef TList<T> TListType;
	
	TComparer _comparer;
	TListType _list;

public:
	TSortedListAdapter()
	{
	}

	TSortedListAdapter(const TSortedListAdapter<T, TComparer> &sortedList) :
		_list(sortedList)
	{
	}

	int Add(const T &value)
	{
		int index = 0;

		for (; index < _list.Count(); ++index)
		{
			if (_comparer.Compare(_list[index], value) < 0)
				continue;

			break;
		}

		_list.InsertAt(value, index);
		return index;
	}

	int Count()
	{
		return _list.Count();
	}

	const T &operator[] (int index)
	{
		return _list[index];
	}

	void Remove(const T &value)
	{
		_list.Remove(value);
	}

	void Clear()
	{
		_list.Clear();
	}
};

template<typename TKey, typename TValue, class TComparer = std::less<TKey>>
class TDictionary
{
	typedef std::map<TKey, TValue, TComparer>	TMap;
	typedef std::pair<TKey, TValue>				TPair;

	TMap _map;

public:
	TDictionary() :
	  _map()
	{
	};

	TDictionary(const TMap &map) :
	  _map(map)
	{
	};

	void Add(TKey key, TValue value)
	{
		_map.insert(TPair(key, value));
	}

	void Remove(TKey key)
	{
		_map.erase(key);
	}

	TPair ElementAt(int index)
	{
		var it = _map.begin();
		for (int i = 0; i < index; ++i)
			it++;

		return *it;
	}

	bool Contains(TKey key)
	{
		return _map.find(key) != _map.end();
	}

	uint32 Count()
	{
		return _map.size();
	}

	void Clear()
	{
		_map.clear();
	}

	TValue operator[] (TKey key)
	{
		TMap::iterator it = _map.find(key);

		if (it == _map.end())
			throw Exceptions::ArgumentInvalid();

		return (*it).second;
	}
};