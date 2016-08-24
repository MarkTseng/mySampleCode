function wait(nSec)
	os.execute("sleep "..tonumber(nSec))
end

function busywait(seconds)
    local _start = os.time()
    local _end = _start+seconds
    while (_end ~= os.time()) do
    end
end
 
print("wait start...")

wait(10)

busywait(10)

print("wait finish...")
