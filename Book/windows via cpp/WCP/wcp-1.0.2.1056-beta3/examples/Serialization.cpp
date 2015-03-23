/// Vectors serialization. WCP library usage example.
///
/// Copyright (c) 2009-2010
/// ILYNS. http://www.ilyns.com
///
/// Copyright (c) 2009-2010
/// Alexander Stoyan
///
/// Follow WCP at:
///      http://wcp.codeplex.com/ - latest releases
///      http://alexander-stoyan.blogspot.com/ - blog about WCP and related stuff

#include <tchar.h>

#include <Windows.h>

#include <algorithm>
#include <functional>

#include "wcp/stlex.h"
#include "wcp/noninstantiable.h"
#include "wcp/constraints.h"
#include "wcp/buffer.h"
#include "wcp/reference.h"

// Type identifiers
enum TypeId
{
    TypeVector2d = 0x1000,
    TypeVector3d,
};


// An archive is an object that serializes some data.
struct IArchive
{
    virtual void Write(const void* p, size_t cb) = 0;
};

// An object header describes an object stored in an archive.
template<TypeId ID, class T>
struct ObjectHeader
{
    ObjectHeader() : typeId(ID), objectSize(sizeof(T)) { }

    const TypeId typeId; // Object type identifier
    const wcp::dword_t objectSize; // Size of stored object in bytes
};

// Describes a common serializable type constraint
struct is_serializable : wcp::noninstantiable
{
    template<class T>
    struct check_constraint : wcp::noninstantiable
    {
        // A type constrained to be either POD or integral
        // and to have at least one data member.
        WCP_ENSURE_TYPE_CONSTRAINT(
            wcp::constraint<T,
            wcp::and_<
            wcp::or_<
            wcp::has_traits_<wcp::is_pod>,
            wcp::has_traits_<wcp::is_integral> >,
            wcp::not_<wcp::has_traits_<wcp::is_empty> >
            >
            > IS_MAINTAINED);
    };
};

// Serializes a serializable object to an archive
template<class T>
inline void Serialize(const T& obj, IArchive& arc)
{
    // Apply predefined serializable type constraints
    WCP_ENSURE_TYPE_CONSTRAINT(wcp::constraint<T, is_serializable> IS_MAINTAINED);

    // Serialize object header
    ObjectHeader<T::TypeId, T> hdr;
    arc.Write(&hdr, sizeof(hdr));

    // Serialize object
    arc.Write(&obj, sizeof(T));
}




#pragma pack(push, 1) // Serializable data is aligned byte 1 byte

// A 2-d vector template.
// Any serializable type is accepted as a vector type.
template<class T>
struct Vector2d
{
    // Vector type is constrained to be a serializable object
    WCP_ENSURE_TYPE_CONSTRAINT(wcp::constraint<T, is_serializable> IS_MAINTAINED);

    // Specify type identifier
    static const TypeId TypeId = TypeVector2d;

    //
    // Vector data
    //

    T x; // x-coordinate
    T y; // y-coordinate
};




// A 3-d vector template.
// Any serializable type is accepted as a vector type.
template<class T>
struct Vector3d
{
    // Vector type is constrained to be a serializable object
    WCP_ENSURE_TYPE_CONSTRAINT(wcp::constraint<T, is_serializable> IS_MAINTAINED);

    // Specify type identifier
    static const TypeId TypeId = TypeVector3d;

    //
    // Vector data
    //

    T x; // x-coordinate
    T y; // y-coordinate
    T z; // z-coordinate
};

#pragma pack(pop)






// Memory arcive implementation
class MemoryArchive : public IArchive
{
    wcp::buffer<> buff;

public:

    // Writes a data to a memory buffer
    void Write(const void* p, size_t cb)
    {
        buff.add(p, cb);
    }

    // Clears an archive
    void Clear()
    {
        buff.clear();
    }

    // Prints out an archive content to a log stream
    void DebugContent() const
    {
        std::tstring line(0x38, TEXT('-'));

        // Print out log header
        std::tclog <<line <<std::endl <<TEXT("         ");
        for(int i = 0; i < 0x10; ++i)
        {
            std::tostringstream ss;
            ss <<std::setfill(TEXT('0')) <<std::setw(2) <<std::hex <<i;
            std::tclog <<ss.str() <<TEXT(' ');
        }

        std::tclog <<std::endl <<TEXT("         ");
        for(int i = 0; i < 0x10; ++i)
            std::tclog <<TEXT("-- ");
        std::tclog <<std::endl;

        // Print out binary content
        size_t counter = 0;
        wcp::reference<size_t> rcounter = counter;
        std::for_each(buff.begin(), buff.end(),
                      std::bind2nd(std::ptr_fun(DbgPrintByte), rcounter));

        // Print out log footer
        std::tclog <<std::endl <<TEXT("Total size: ") <<buff.size()
                   <<std::endl <<std::endl;
    }

private:

    // Prints out a byte in hexamidal representation
    static void DbgPrintByte(wcp::byte_t b,
                             wcp::reference<size_t> rcounter)
    {
        // Print out offset info
        if((rcounter.deref() % 0x10) == 0)
            std::tclog <<std::setfill(TEXT('0')) <<std::hex
                       <<std::setw(8) <<rcounter.deref();

        // Print out byte in hex
        std::tostringstream ss;
        ss <<std::setfill(TEXT('0')) <<std::setw(2) <<std::hex <<b;
        std::tclog <<TEXT(' ') <<ss.str();

        if((++rcounter.deref() % 0x10) == 0)
            std::tclog <<std::endl;
    }
};




int _tmain(int argc, TCHAR* argv[])
{
    // Declare 4 kinds of vector. Note: Only non-empty POD or integral types
    // are acceptable as vector types.

    Vector2d<int> v1 = { 10, 10 };
    Vector2d<float> v2 = { 3.1415f, 74.1898f };
    Vector2d<wcp::ulonglong_t> v3 = { ~0, ~0 };
    Vector3d<Vector2d<short> > v4 =
    {
        { 0xff, 0xee },
        { 0xdd, 0xcc },
        { 0xbb, 0xaa }
    };

    //// This will cause a compile time error,
    //// because a type constrained is violated.
    //Vector2d<std::tstring> v5 = { L"", L"" };

    // Serialize vectors to memory archive
    // and debug memory archive content

    MemoryArchive arc;

    Serialize(v1, arc);
    arc.DebugContent();
    arc.Clear();

    Serialize(v2, arc);
    arc.DebugContent();
    arc.Clear();

    Serialize(v3, arc);
    arc.DebugContent();
    arc.Clear();

    Serialize(v4, arc);
    arc.DebugContent();
    arc.Clear();

    return EXIT_SUCCESS;
}
