
--local html_fd = io.open("test_parser.html")
local html_fd = io.open("台中市北屯區.html")
local html_context
html_context = html_fd:read("*a")



--for x,y,z in string.gmatch(html_context,"(%b<>)(.-)(%b<>)")
for x,y,z in string.gmatch(html_context,"(<%s*li%s*%s*>)(.-)(<%s*/%s*li%s*%s*>)")
do
	print("[li tag parser_start]")
	print("group 1: "..x)
  	y = string.gsub(y,"&nbsp;","")
	print("group 2: "..y)
	print("group 3: "..z)
end

html_fd:close()

