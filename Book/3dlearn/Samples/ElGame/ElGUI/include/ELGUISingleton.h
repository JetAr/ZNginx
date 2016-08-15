#ifndef __ELGUISingleton_H__
#define __ELGUISingleton_H__

namespace ELGUI
{
	template <typename T> class Singleton
	{
	protected:
		static T* ms_Singleton;

	public:
		Singleton()
		{
			assert(!ms_Singleton);
			ms_Singleton = static_cast<T*>(this);
		}

		virtual ~Singleton()
		{
			assert( ms_Singleton );
			ms_Singleton = 0;
		}

		static T& getSingleton()
		{
			assert( ms_Singleton );
			return (*ms_Singleton);
		}

		static T* getSingletonPtr()
		{
			return (ms_Singleton);
		}
	};
}

#endif //__ELGUISingleton_H__