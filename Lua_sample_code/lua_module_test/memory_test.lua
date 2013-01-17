#!/usr/local/bin/lua

-- create a table to test memory
t={}

for i=1,50000000 do
	t[i] = i
end

os.execute("sleep 5")
-- to free memory when a table will not use forever

t = nil
collectgarbage()

