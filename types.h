#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
using namespace std;

// C style
// typedef int T;

// C++11 style
using TI = int;
using TD = double;
using TS = string;

// XT must be 32bit integer in Windows and 64bit in Linux
#if defined(_WIN32) || defined(_WIN64)
    using XT = int;
#else
    using XT = long;
#endif

using Ref = long;

/// TIPOS PARA BTREE ///

using obj_id_t      =   long;
using use_count_t   =   long;
using tree_order_t  =   int;
using tree_height_t =   int;
using btree_key_t   =   int;
using btree_pos_t   =   int;
using bree_size_t   =   int;


#endif // __TYPES_H__
