#!/usr/local/bin/lua

-- create a table to test memory
t={}

for i=1,10000000 do
	t[i] = i
end

-- to free memory when a table will not use forever
t = nil
collectgarbage()

