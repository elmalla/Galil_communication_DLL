#include <windows.h>
#include <sstream>
#include <new>
 // OutputDebugString
#include "debugnew.h"

#if !defined NDEBUG

bool Tracer::Ready = false;

void * operator new (size_t size, char const * file, int line)
{
	void * p = malloc (size);
	if (Tracer::Ready)
		NewTrace.Add (p, file, line);
	return p;
}

void operator delete (void * p, char const * file, int line)
{
	if (Tracer::Ready)
		NewTrace.Remove (p);
	free (p);
}

void * operator new (size_t size)
{
	void * p = malloc (size);
	if (Tracer::Ready)
		NewTrace.Add (p, "?", 0);
	return p;
}

void operator delete (void * p)
{
	if (Tracer::Ready)
		NewTrace.Remove (p);
	free (p);
}

Tracer::Tracer () 
: _lockCount (0) 
{
	Ready = true;
}

Tracer::~Tracer ()
{
	Ready = false;
	Dump ();
}

void Tracer::Add (void * p, char const * file, int line)
{
	if (_lockCount > 0)
		return;
	Tracer::Lock lock (*this);
	_map [p] = Entry (file, line);
}

void Tracer::Remove (void * p)
{
	if (_lockCount > 0)
		return;

	Tracer::Lock lock (*this);

	iterator it = _map.find (p);
	if (it != _map.end ())
	{
		char const * file = it->second.File ();
		int line = it->second.Line ();
		_map.erase (it);
	}
}

void Tracer::Dump ()
{
	if (_map.size () != 0)
	{
		::OutputDebugString ("*** Memory leak(s):\n");
		for (iterator it = _map.begin (); it != _map.end (); ++it)
		{
			char const * file = it->second.File ();
			int line = it->second.Line ();
			int addr = reinterpret_cast<int> (it->first);
			std::stringstream out;
			//out << "0x" << std::hex << addr << ": " 
			//	<< file << ", line " << std::dec << line << std::endl;
			char buffer1 [10];
			char buffer2 [8];
			out << "0x" << itoa (addr, buffer1, 16) << ": "
				<< file << ", line " << itoa (line, buffer2, 10) << std::endl;
			::OutputDebugString (out.str ().c_str ());
		}
		::OutputDebugString ("\n");
	}
}

#endif