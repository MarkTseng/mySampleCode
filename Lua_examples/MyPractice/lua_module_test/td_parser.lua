#!/usr/local/bin/lua

local html_td = "<td align=\"center\" bgcolor=\"#FFFfff\" nowrap>13:30</td>"

print(html_td)
print("match:\n")
links = {}
for k,v,s in string.gmatch(html_td, "(<td%s+.*>)(.*)(</td>)") do
       --links[k] = v
		print("%%1: "..k)
		print("%%2: "..v)
		print("%%3: "..s)
end
