 local Error_Type = {
	bussiness = "Bussiness",
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
	--print ("hello world")
	if stk.last < stk.low then
		error_type = Error_Type.bussiness
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
	--print("szl2_order_stat")
	--print(stk.m_wStkID)
 	return nil
 end

 function handle_szl2_order_five( stk )
	--print ("szl2_order_five")
	--print (stk.nIndex)
 	return nil
 end

 function handle_szl2_trade_five( stk )
	--print ("szl2_trade_five")
	--print (stk.nIndex)
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

function handle_future(future)
	if(future.last_day_oi > 0) then
		error_type = Error_Type.bussiness
		error_level = Error_Type.error
		error_info = "future last day oi > 0"
		return FormatErrorString(error_type, error_level, error_info)
	end
	if(future.last_settle_price > 0) then
		
	end
	return nil
end

function handle_did_template_100000(data)
	
 end
  
