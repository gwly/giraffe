local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_IOPV_INFO//ETFLOF净值
{
uint32	STKID;//股票ID
dtime	UpdateTime;//数据更新GMT时间
pfloat	Price;//成交价格
}T_IOPV_INFO;
#pragma unpack()
]]
