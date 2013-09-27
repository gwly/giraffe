local ffi = require("ffi")

ffi.cdef[[
#pragma pack(1)

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;
typedef char byte;
typedef uint64 dtime;
typedef int64 pfloat;
typedef int64 vfloat;

#pragma pack()
]]
