complex = {}

function complex.new (r, i) return {r=r, i=i} end

-- defines a constant 'i'
complex.i = complex.new(0, 1)

function complex.add (c1, c2)
	return complex.new(c1.r + c2.r, c1.i + c2.i)
end

function complex.sub (c1, c2)
	return complex.new(c1.r - c2.r, c1.i - c2.i)
end

function complex.mul (c1, c2)
	return complex.new(c1.r*c2.r - c1.i*c2.i,
c1.r*c2.i + c1.i*c2.r)
end

local function inv (c)
	local n = c.r^2 + c.i^2
	return complex.new(c.r/n, -c.i/n)
end

function complex.div (c1, c2)
	return complex.mul(c1, inv(c2))
end
return complex
