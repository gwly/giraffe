local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_BUY_SELL_TICK_INFO//逐笔信息
{
uint32	STKID;//股票ID
dtime	cjTime;//数据成交GMT间
pfloat	Price;//成交价格
vfloat	AllBuyVol;//成交量
byte	InOutFlag;//内外盘标记
uint64	BuyOrderId;//买委托订单号
uint64	SellOrderId;//卖委托订单号
}T_BUY_SELL_TICK_INFO;
#pragma unpack()
]]
