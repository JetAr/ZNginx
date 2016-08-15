#ifndef __ElSingleton_H__
#define __ElSingleton_H__

#include "ElDefines.h"

#define ElDeclareSingleton(classname) \
	public: \
		static classname& getSingleton(); \
		static classname* getSingletonPtr(); \
		static void shutdown(); \
	protected: \
		static void ensureInstantiated(); \
	protected: \
		static classname* msSingleton;

#define ElImplementSingleton(classname) \
	classname* classname::msSingleton = NULL; \
	classname& classname::getSingleton() \
	{ \
		ensureInstantiated(); \
		return *msSingleton; \
	}\
	classname* classname::getSingletonPtr() \
	{ \
		ensureInstantiated(); \
		return msSingleton; \
	} \
	void classname::shutdown() \
	{ \
		delete msSingleton; \
	} \
	void classname::ensureInstantiated() \
	{ \
		if (msSingleton == NULL) \
			msSingleton = new classname; \
	}

#define ElShutDownSingleton(classname) \
	classname::shutdown();

#endif //__ElSingleton_H__