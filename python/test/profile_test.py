#!//usr/bin/env python
import jedi
import cProfile, pstats, StringIO, time, os, sys

def logged(fn): 
    def with_logging(*args, **kwargs): 
        t1 = time. time()
        result = fn(*args, **kwargs)
        t2 = time. time()
        print ("@timefn:" + fn. func_name + " took " + str(t2 - t1) + " seconds" )
        return result 
    return with_logging

@logged
def test_decorator():
    i = 10000000;
    while (i>1):
        i = i - 1
    print "my decorator"

test_decorator()
 
pr = cProfile.Profile()
pr.enable()
# ... do something ...
test_decorator()
pr.disable()
s = StringIO.StringIO()
sortby = 'cumulative'
ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
ps.print_stats()
print s.getvalue()


