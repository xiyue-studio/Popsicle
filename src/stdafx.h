typedef unsigned char uint8;
typedef char int8;

typedef unsigned int uint32;
typedef int int32;

typedef unsigned long long uint64;
typedef long long int64;

#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER // for MSVC

#define forceinline __forceinline
#include <winsock.h>

#elif defined __GNUC__ // for gcc on Linux/Apple OS X

#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#define SOCKET int
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define forceinline __inline__ __attribute__((always_inline))

#else

#define forceinline
#define SOCKET int

#endif

#define element_of(o) (sizeof((o)) / sizeof((o)[0]))
#include <string.h>
#include <string>

struct PxString
{
	PxString()
	{
		str = NULL;
		length = 0;
	}

	PxString(const char* str, size_t length)
	{
		this->str = str;
		this->length = length;
	}

	bool operator<(const PxString& r) const
	{
		return _strnicmp(this->str, r.str, this->length > r.length ? this->length : r.length) < 0;
	}

	const char* str;
	size_t length;
};

struct StringLess
{
	bool operator()(const std::string& l, const std::string& r)
	{
		return _stricmp(l.c_str(), r.c_str()) < 0;
	}
};

#define invalidSocket SOCKET(0)
