template<typename T>
ElArray<T>::ElArray()
: _base(NULL)
, _size(0)
{

}

template<typename T>
ElArray<T>::~ElArray()
{
	ElSafeDeleteArray(_base);
}

template<typename T>
inline T* ElArray<T>::base()
{
	return _base;
}

template<typename T>
inline int ElArray<T>::size()
{
	return _size;
}

template<typename T>
inline bool ElArray<T>::empty()
{
	return _size <= 0;
}

template<typename T>
inline void ElArray<T>::assign(int size)
{
	ElSafeDeleteArray(_base);
	_base = ElNew T[size];
	_size = size;
}

template<typename T>
inline void ElArray<T>::swap(ElArray<T>& rhs)
{
	T* t_base = _base;
	_base = rhs._base;
	rhs._base = t_base;

	int t_size = _size;
	_size = rhs._size;
	rhs._size = t_size;
}

template<typename T>
inline void ElArray<T>::clear()
{
	ElSafeDeleteArray(_base);
	_size = 0;
};

template<typename T>
inline T& ElArray<T>::operator [](int idx)
{
	return _base[idx];
}
