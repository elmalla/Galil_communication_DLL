#if !defined DEBUGNEW_H
#define DEBUGNEW_H
#pragma warning (disable:4786)

void * operator new (size_t size, char const * file, int line);
void operator delete (void * p, char const * file, int line);
void * operator new (size_t size);
void operator delete (void * p);

#include <map>

class Tracer
{
private:
	class Entry
	{
	public:
		Entry (char const * file, int line)
			: _file (file), _line (line)
		{}
		Entry ()
			: _file (0), _line (0)
		{}
		char const * File () const { return _file; }
		int Line () const { return _line; }
	private:
		char const * _file;
		int _line;
	};
	class Lock
	{
	public:
		Lock (Tracer & tracer)
			: _tracer (tracer)
		{
			_tracer.lock ();
		}
		~Lock ()
		{
			_tracer.unlock ();
		}
	private:
		Tracer & _tracer;
	};
	typedef std::map<void *, Entry>::iterator iterator;
	friend class Lock;
public:
	Tracer ();
	~Tracer ();
	void Add (void * p, char const * file, int line);
	void Remove (void * p);
	void Dump ();

	static bool Ready;
private:
	void lock () { _lockCount++; }
	void unlock () { _lockCount--; }
private:

	std::map<void *, Entry> _map;
	int _lockCount;

};

extern Tracer NewTrace;

#endif
