require "mylib"

local lib = mylib

dir_table = lib.dir("/home/mark");

for key,value in ipairs(dir_table)
do
	print(key,value)
end
