#ifndef __ElArray_H__
#define __ElArray_H__

#include "ElDefines.h"

template<typename T> class ElArray
{
public:
	ElArray();
	virtual ~ElArray();

	T*		base();
	int		size();
	bool	empty();
	void	assign(int size);
	void	swap(ElArray<T>& rhs);
	void	clear();
	T&		operator[](int idx);

protected:
	T*		_base;
	int		_size;
};

#include "ElArray.inl"

#endif //__ElArray_H__