local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_MMP_INFO//涔板崠鐩樻寕鎾ゅ崟淇℃伅
{
uint32	STKID;//鑲＄エID
dtime	updateTime;//鏁版嵁鏇存柊GMT鏃堕棿
pfloat	Price;//浠锋牸
int32	Pos;//妗ｄ綅
int32	Vol;//鎸傛挙鍗曢噺
}T_MMP_INFO;
#pragma pack()
]]
