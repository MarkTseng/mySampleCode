require "mylib"

dir_table = {};

dir_table = mylib.dir("/home/mark");

for key,value in ipairs(dir_table)
do
	print(key,value)
end
