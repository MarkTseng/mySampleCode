#!/usr/local/bin/lua

function GetStockPrice(StockNumber)
	local StockNum = StockNumber;
	local StockLink = "http://tw.stock.yahoo.com/q/q?s="..StockNum
	local httpObj = require("socket.http")
	--[[
	http.request(url [, body])
	http.request{
	  url = string,
	  [sink = LTN12 sink,]
	  [method = string,]
	  [headers = header-table,]
	  [source = LTN12 source],
	  [step = LTN12 pump step,]
	  [proxy = string,]
	  [redirect = boolean,]
	  [create = function]
	}

	]]--

	--b,h,c = httpObj.request("http://www.kimo.com.tw")
	b,h,c = httpObj.request(StockLink)
	-- b returns some useless page telling about the denied access,
	-- h returns authentication information
	-- and c returns with value 401 (Authentication Required)

	-- String to Table
	local HTML_Table = {}
	local i=0
	local HTMLBODY = b
	while i < 250 do

		local StringTEMP
		s1 = string.find(HTMLBODY,"\n")
		StringTEMP = string.sub(HTMLBODY,0,s1)
		--print("line :"..s1..StringTEMP)
		HTML_Table[#HTML_Table+1] = StringTEMP
		HTMLBODY = string.sub(HTMLBODY,s1+1)
		--print(HTMLBODY)
		i = i + 1
	end

	-- Start to parser Stock price in HTML
	local Stock = {}
	for k,v in pairs(HTML_Table) do
		--print(k,v)

		for k,price,s in string.gmatch(v, "(<td%s+.*nowrap>)(.*)(</td>)") do
			Stock[#Stock+1] = price
			--print(price)
		end
	end


	--Print Stock information
	TradePrice = string.sub(Stock[2],4,-5)
	--print("股票代號"..StockNum.."\n")
	print("時間\t成交\t買進\t賣出\t漲跌\t張數\t昨收\t開盤\t最高\t最低")
	print(Stock[1].."\t"..string.sub(Stock[2],4,-5).."\t"..Stock[3].."\t"..Stock[4].."\t"..string.format("%3.2f",TradePrice-Stock[6]).."\t"..Stock[5].."\t"..Stock[6].."\t"..Stock[7].."\t"..Stock[8].."\t"..Stock[9].."\n")

end



--[[
StockName = Stock number
]]--

local StockConfFileName = arg[1] or "stock_list.txt"
local Stock_Table = {};

-- open file
FD_StockList = io.open(StockConfFileName,"r")

-- read Stock_list to Stock_Table
for line in FD_StockList:lines() do
	--print(line)
	for k,v in string.gmatch(line,"(.+)%s+=%s+(.+)") do
		--print(k.." is "..v)
		Stock_Table[v] = k
	end
end

-- close file
FD_StockList:close()


-- List Stock_Table

for k,v in pairs(Stock_Table) do
	print(k,v)
	GetStockPrice(k)
end
