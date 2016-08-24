local http = require("socket.http")
local MyZone = require("ZoneArea")

local searchZoneArea

function wait(seconds)
	local _start = os.time()
	local _end = _start+seconds
	while (_end ~= os.time()) do	end
end

--[[
function wait(nSec)
	os.execute("sleep "..tonumber(nSec))
end
]]--

function getZoneAreaPage(keyWord)
	local savePageFD
	local searchPatern = string.format("County=&District=&Rooms=&PriceMin=&PriceMax=&AreaMin=&AreaMax=&Purpose=&CaseType=&BuildAge=&CaseFloor=&DirFace=&ParkingSpace=&KeyWord=%s&Group=&ListMode=PhotosAndWords&PageCount=2000&CurrentPage=1&Current",keyWord)
	print(searchPatern)
	b, c, h = http.request("http://yungching.housefun.com.tw/Buy/Ashx/ShowList.ashx",searchPatern)

	savePageFD = io.open(keyWord..".html","w")
	savePageFD:write(b)
	savePageFD:close()
	print(savePageFD)
	--print(b)

end
for city,area in pairs(MyZone)
do 
	for _,area in pairs(area)
	do 
		searchZoneArea = string.format("%s%s",city,area)
		print(searchZoneArea)
		getZoneAreaPage(searchZoneArea)
		wait(30)
	end
end

