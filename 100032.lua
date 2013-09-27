local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_MMP_INFO//买卖盘挂撤单信息
{
uint32	STKID;//股票ID
dtime	updateTime;//数据更新GMT时间
pfloat	Price;//价格
int32	Pos;//档位
int32	Vol;//挂撤单量
}T_MMP_INFO;
#pragma pack()
]]
