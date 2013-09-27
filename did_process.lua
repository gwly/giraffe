require("base_type")

local ffi = require("ffi")

local C = ffi.C

function process_did(template_id)
	local template
	if template_id == "100000" then
		template = require("100000")
		print("hello")
	elseif template_id == "100001" then
		template = require("100001")
	elseif template_id == "100002" then
		template = require("100002")
	elseif template_id == "100012" then
		template = require("100012")
	elseif tempalte_id == "100030" then
		template = require("100030")
	elseif template_id == "100032" then
		template = require("100032")
	end
end
