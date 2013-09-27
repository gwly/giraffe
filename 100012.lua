local ffi = require("ffi")
ffi.cdef[[
#pragma pack(1)
typedef struct T_CBT_MARKET//现券交易衍生指标
{
uint32	STKID;//品种ID
dtime	time;//成交GMT时间
pfloat	todayaccruedint;//日间应记利息
pfloat	lastdayaccruedint;//日终应记利息
pfloat	macaulayduration;//麦氏久期
pfloat	adjifiedduration;//修正久期
pfloat	adjifiedduration_ex;//行权修正久期
pfloat	convexity;//凸性
pfloat	convexity_ex;//行权凸性
pfloat	intrateduration;//利率久期
pfloat	intrateduration_ex;//行权利率久期
pfloat	spreadduration;//利差久期
pfloat	spreadduration_ex;//行权利差久期
pfloat	intrateconvexity;//利率凸性
pfloat	intrateconvexity_ex;//行权利率凸性
pfloat	spreadconvexity;//利差凸性
pfloat	spreadconvexity_ex;//行权利差凸性
pfloat	ytmcentralbankrules;//到期收益率_央行规则
pfloat	yieldtoexutioncentral;//行权收益率_央行规则
pfloat	bondchange;//转股价值
pfloat	ConversionPremiumRatio;//转股溢价率
pfloat	ValueOfNetDebt;//纯债价值
pfloat	ValueOfNetDebtRatio;//纯债溢价率
pfloat	wholeprice;//全价
}T_CBT_MARKET;
#pragma pack()
]]
