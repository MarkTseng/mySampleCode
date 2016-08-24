#!/usr/local/bin/lua
--[[
StockName = Stock number
]]--

local StockConfFileName = arg[1] or "stock_list.txt"
local Stock_Table = {};

-- open file
FD_StockList = io.open(StockConfFileName,"r")

-- read Stock_list to Stock_Table
while true do
	Stock_list = FD_StockList:read("*l")
	if(Stock_list == nil) then break end
 
	for k,v in string.gmatch(Stock_list,"(.+)%s+=%s+(.+)") do
		--print(k.." is "..v)
		Stock_Table[v] = k
	end
end 

-- close file
FD_StockList:close()


-- List Stock_Table

for k,v in pairs(Stock_Table) do
	print(k,v)
end

