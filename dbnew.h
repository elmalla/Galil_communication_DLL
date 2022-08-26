#if !defined DBNEW_H
#define DBNEW_H

#if !defined NDEBUG
#include "debugnew.h"

#define new new(__FILE__, __LINE__)

#endif

#endif