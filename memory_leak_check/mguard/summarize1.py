import sys, os, struct, ctypes, argparse

def ERROR(x): print "!!ERROR!! "+x

def ERROR(x,exception):
    print "!!ERROR!! "+x
    print "    => ", exception

def WARNING(x): print "!!WARNING!! "+x

class MSummary:
    pass



def main():
    ver=3
    fstr=":X"
    inputFile=sys.argv[1]
    headers=1
    line_count=0
    out=None
    if len(sys.argv) >2:
        out=open(sys.argv[2],'w')

    smap={}
    mmap={}
    fmap={}

    for line in open(inputFile):
        line=line.strip()

        if line.startswith("MGUARD version '1.0'"):
            ver=1
        elif line.startswith("MGUARD version '1.01'"):
            ver=2
            fstr=":F"

        if headers !=0 and ver>1 and line.startswith("========@"):
            out.write("\n")
            out.write(':[TYPE]:[BACKTRACE_ID]:[TOTAL_UNFREED_SIZE]:[XCOUNT]:[BACKTRACE]'+"\n")
            out.write(line+"\n")
            headers=0
        elif headers!=0 and ver==1 and line_count>=7:
            out.write("\n")
            headers=0

        if headers:
            out.write(line+"\n")

        if line.startswith(fstr):
            tokens=line.split(":")
            k=tokens[2].strip()
            smap[k]=line
            
        line_count+=1

    for line in open(inputFile):
        line=line.strip()
        if line.startswith(":M"):
            tokens=line.split(":")
            k=tokens[2].strip()
            mmap[k]=line
            
#            if ver>2:
#                m.mtype=int(tokens[4])

    for line in open(inputFile):
        line=line.strip()
        if line.startswith(":F"):
            tokens=line.split(":")
            k=int(tokens[2].strip(),16)
            fmap[k]=line

    keys=mmap.keys()
    keys.sort()

    total_mem=0
    for k in keys:
##        print '>>>> '+str
        if out is not None:
            tokens=mmap[k].split(":")
            str=mmap[k]+"|"+smap[tokens[6]]
            out.write(str)
            out.write("\n")
            intk=(int(k,16)+1)
            
            if fmap.has_key(intk):
                out.write("  "+fmap[intk])
                out.write("\n")


    if out is not None:
        out.close()




if __name__ == "__main__":
    main()
