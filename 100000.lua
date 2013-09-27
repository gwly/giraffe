local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_BUY_SELL_INFO//买卖盘信息
{
uint32	STKID;//股票ID
dtime	updateTime;//数据更新GMT时间
uint32	BuyPriceNum;//买方委托价位数
uint32	BuyWithdrawCount;//委买撤单总笔数
vfloat	BuyWithdrawVol;//委买撤单总量
vfloat	BuyWithdrawAmount;//委买撤单总金额
uint32	SellPriceNum;//卖方委托价位数
uint32	SellWithdrawCount;//委卖撤单总笔数
vfloat	SellWithdrawVol;//委卖撤单总量
vfloat	SellWithdrawAmount;//委卖撤单总金额
uint32	BuyCount[10];//委买笔数1到10
uint32	SellCount[10];//委卖笔数1到10
}T_BUY_SELL_INFO;
#pragma pack()
]]
