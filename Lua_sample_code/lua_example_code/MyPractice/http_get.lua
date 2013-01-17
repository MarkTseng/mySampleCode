#!/usr/local/bin/lua

local StockNum = arg[1];
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
while i < 235 do

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

	for k,key,s in string.gmatch(v, "(<td%s+.*nowrap>)(.*)(</td>)") do
		Stock[#Stock+1] = key
		--print(key)
	end
end


--Print Stock information

print("股票代號"..StockNum.."\n")
print("時間\t成交\t買進\t賣出\t漲跌\t張數\t昨收\t開盤\t最高\t最低\n")
print(Stock[1].."\t"..string.sub(Stock[2],4,-5).."\t"..Stock[3].."\t"..Stock[1].."\t"..Stock[1].."\t"..Stock[1].."\t"..Stock[1].."\t"..Stock[1].."\t"..Stock[1].."\t"..Stock[1].."\n")

-- open a file descript
--[[
local FO_image = io.open("3454.html","w")

FO_image:write(b)

FO_image:close()
]]--