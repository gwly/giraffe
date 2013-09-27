local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_ETF_INFO//ETF申购赎回信息
{
uint32	STKID;//股票ID
dtime	UpdateTime;//更新时间
uint32	EtfBuyNumber;//ETF申购笔数
vfloat	EtfBuyAmount;//ETF申购数量
pfloat	EtfBuyMoney;//ETF申购金额
uint32	EtfSellNumber;//ETF赎回笔数
vfloat	EtfSellAmount;//ETF赎回数量
pfloat	EtfSellMoney;//ETF赎回金额
}T_ETF_INFO;
#pragma unpack()
]]
