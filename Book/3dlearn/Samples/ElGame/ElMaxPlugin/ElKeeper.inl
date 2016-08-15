template<typename T>
void ElKeeper<T>::Clear()
{
	mArray.clear();
}

template<typename T>
bool ElKeeper<T>::Add(T* obj)
{
	if (!obj)
		return false;

	int numObj = mArray.size();
	for (int i = 0; i < numObj; ++i)
	{
		if (mArray[i] == obj)
			return false;
	}

	mArray.push_back(obj);

	return true;
}

template<typename T>
int ElKeeper<T>::GetID(T* obj)
{
	int numObj = mArray.size();
	for (int i = 0; i < numObj; ++i)
	{
		if (mArray[i] == obj)
			return i;
	}

	return -1;
}

template<typename T>
int ElKeeper<T>::Count()
{
	return mArray.size();
}

template<typename T>
T* ElKeeper<T>::Get(int id)
{
	return mArray[id];
}