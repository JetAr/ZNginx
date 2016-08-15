#ifndef __ElRTTI_H__
#define __ElRTTI_H__

/**
 *	run-time type information
 */
class ElRTTI
{
public:
	ElRTTI(const char* name, const ElRTTI* baseRTTI) : mName(name), mBaseRTTI(baseRTTI) {}

	const char* getName() const { return mName; }
	const ElRTTI* getBaseRTTI() const { return mBaseRTTI; }

protected:
	const char* mName;
	const ElRTTI* mBaseRTTI;
};

#define ElDeclareRootRTTI(classname) \
	public: \
		static const ElRTTI msRTTI; \
		virtual const ElRTTI* getRTTI() const { return &msRTTI; } \
		static bool isExactKindOf(const ElRTTI* RTTI, \
			const classname* object) \
		{ \
			if (!object) \
				return false; \
			return object->isExactKindOf(RTTI); \
		} \
		bool isExactKindOf(const ElRTTI* RTTI) const \
		{ \
			return getRTTI() == RTTI; \
		} \
		static bool isKindOf(const ElRTTI* RTTI, \
			const classname* object) \
		{ \
			if (!object) \
				return false; \
			return object->isKindOf(RTTI); \
		} \
		bool isKindOf(const ElRTTI* RTTI) const \
		{ \
			const ElRTTI* tmp = getRTTI(); \
			while (tmp) \
			{ \
				if (tmp == RTTI) \
					return true; \
				tmp = tmp->getBaseRTTI(); \
			} \
			return false; \
		} \
		static classname* dynamicCast(const ElRTTI* RTTI, \
			const classname* object) \
		{ \
			if (!object) \
				return 0; \
			return object->dynamicCast(RTTI); \
		} \
		classname* dynamicCast(const ElRTTI* RTTI) const \
		{ \
			return isKindOf(RTTI) ? (classname*)this : 0; \
		}

#define ElDeclareRTTI \
	public: \
		static const ElRTTI msRTTI; \
		virtual const ElRTTI* getRTTI() const { return &msRTTI; }

#define ElImplementRootRTTI(classname) \
	const ElRTTI classname::msRTTI(#classname, 0)

#define ElImplementRTTI(classname, baseclassname) \
	const ElRTTI classname::msRTTI(#classname, &baseclassname::msRTTI)

#define ElIsExactKindOf(classname, object) \
	classname::isExactKindOf(&classname::msRTTI, object)

#define ElIsKindOf(classname, object) \
	classname::isKindOf(&classname::msRTTI, object)

#define ElStaticCast(classname, object) \
	((classname*)object)

#define ElDynamicCast(classname, object) \
	((classname*)classname::dynamicCast(&classname::msRTTI, object))

#endif //__ElRTTI_H__