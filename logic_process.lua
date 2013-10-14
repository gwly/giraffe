local Error_Type = {
   business = "Business",
   system = "System"
}
local Error_Level = {
   warning = "Warning",
   error = "Error"
}

 
local str_format = string.format
function FormatErrorString(error_type,error_level,error_info)
	return str_format("%s:%s:%s",error_type, error_level, error_info)
end

local error_type
local error_level
local error_info

function handle_stk_dyna(stk)
	if stk.last < stk.low then
		error_type = Error_Type.business
		error_level = Error_Level.warning	
		error_info = "last < low"
		return FormatErrorString(error_type,error_level,error_info)
	end
	return nil
end

function handle_stk_static(stk)
   if stk.last_close < 0 then
	   error_type = Error_Type.system
	   error_level = Error_Level.error
	   error_info = "last close < 0"
	   return FormatErrorString(error_type, error_level, error_info)
   end
   return nil
end

function handle_shl2_mmpex(stk)
	if stk.avg_buy_price >= 0 then
		error_type = Error_Type.system
		error_level = Error_Level.error
		error_info = "avg_buy_price > 0"
		return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_shl2_mmp(stk)
	if stk.mmp>= 0 then
		error_type = Error_Type.system
		error_level = Error_Level.error
		error_info = "queue element value >= 0"
		return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_szl2_order_stat( stk )
	if stk.m_wStkID < 0 then
		error_type = Error_Type.system
		error_level = Error_Level.error
		error_info = "stkid  < 0"
		return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_szl2_order_five( stk )
    if stk.dwPrice < 0 then
        error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "stk price < 0"
        return FormatErrorString(error_type, error_level, error_info)
    end
	return nil
end

function handle_szl2_trade_five( stk )
    if stk.dwPrice < 0 then
        error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "stk price < 0"
        return FormatErrorString(error_type, error_level, error_info)
    end
	return nil
end

function handle_equity(equity)
	if equity.m_fProfit < 0 then
		error_type = Error_Type.system
		error_level = Error_Level.error
		error_info = "equity profit < 0"
		return FormatErrorString(error_type, error_level, error_info)
	end	
	return nil
end

function handle_fund(fund)
	if fund.m_fIOPV < 0 then
        error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "fund IOPV < 0"
        return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_warrant(warrant)
	if warrant.m_fStrikePrice < 0 then
		error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "fund IOPV < 0"
        return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_bond(bond)
	if bond.m_fAccruedInt < 0 then
        error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "bond accrued interest < 0"
        return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_Cnvt(cnvt)
	if cnvt.m_fStrikePrice < 0 then
        error_type = Error_Type.system
        error_level = Error_Level.error
        error_info = "cnvt StrikePrice < 0"
        return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_future(future)
	if future.last_settle_price < 0 then
		error_type = Error_Type.business
		error_level = Error_Type.error
		error_info = "future last settle price < 0"
		return FormatErrorString(error_type, error_level, error_info)
	end
	return nil
end

function handle_trust(trust)
	if trust.m_dwfAsset < 0 then
        error_type = Error_Type.business
        error_level = Error_Type.error
        error_info = "trust wfAsset < 0"
        return FormatErrorString(error_type, error_level, error_info)	
	end 
	return nil
end

function handle_hkdyna(stk)
	if stk.m_dwIEPrice < 0 then
        error_type = Error_Type.business
        error_level = Error_Type.error
        error_info = "stk IE Price < 0"
        return FormatErrorString(error_type, error_level, error_info)	
	end
	return nil
end

function handle_iopv(iopv)
   if(iopv >= 0.0) then
    error_type = Error_Type.business
    error_level = Error_Level.error
    error_info = "iopv > 0.0"
    return FormatErrorString(error_type, error_level, error_info)
   end
   return nil
end

function handle_t_buy_sell_info(info)
	local buy_count = info.BuyCount
	for i=1,10 do
		if buy_count < 0 then
			error_type = Error_Type.business
			error_level = Error_Level.error
			error_info = "buy count < 0"
			return FormatErrorString(error_type, error_level,
									 error_info)	
		end
		buy_count = buy_count + 1	
	end
	return nil
end 

function handle_t_buy_sell_tick_info(info)
	if info.Price < 0 then
		error_type = Error_Type.business
		error_level = Error_Level.error
		error_info = "price < 0"
		return FormatErrorString(error_type, error_level,
								error_info)
	end	
	return nil
end

function handle_t_iopv_info(info)
    if info.Price < 0 then
        error_type = Error_Type.business
        error_level = Error_Level.error
        error_info = "price < 0"
        return FormatErrorString(error_type, error_level,
                                error_info)
    end
	return nil
end

function handle_t_cbt_market(info)
    if info.wholeprice < 0 then
        error_type = Error_Type.business
        error_level = Error_Level.error
        error_info = "wholeprice < 0"
        return FormatErrorString(error_type, error_level,
                                error_info)
    end
	return nil
end

function handle_t_etf_info(info)
	if info.EtfBuyMoney < 0 then
		error_type = Error_Type.business
		error_level = Error_Level.error
		error_info = "etf buy money < 0"
		return FormatErrorString(error_type, error_level,
								error_info)	
	end
	return nil
end

function handle_t_mmp_info(info)
	if info.Vol < 0 then
		error_type = Error_Type.business
		error_level = Error_Level.error
		error_info = "mmp vol < 0"
		return FormatErrorString(error_type, error_level,
								error_info)	
	end
	return nil
end
