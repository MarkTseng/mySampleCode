t = {10, 20, 30}

function values (t)
	local i = 0
	return function () i = i + 1; return t[i] end
end

for element in values(t) do
	print(element)
end

a = {}
a["test1"] = {"one", "two", "three"}
a["test2"] = {"one", "two", "three"}

local function iter (a, i)
	i = i + 1
	local v = a[i]
	if v then
		return i, v
	end
end

function getpairs (a)
	return iter, a, 0
end

for i, v in getpairs(a) do
	print(i, v)
end

