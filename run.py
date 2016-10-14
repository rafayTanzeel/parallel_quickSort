import subprocess
from numpy import linspace
from time import sleep
import os

def main():
    fileName = ["driver-sort", "driver-compute", "driver-reduce"]
    grainSize=linspace(100, 40000000, 20)
    threading={"quadThreadSep.txt":4}
    for key, value in threading.iteritems():
        for f in fileName:
            os.remove(f+'-'+key) if os.path.exists(f+'-'+key) else None
            with open(f+'-'+key, 'a') as outfile:
                outfile.write("Grainsize,Time\n")
                for grain in grainSize:
                    args="./{} --particles 40000000 --trials 5 --grainsize {} --threads {}".format(f, grain, value)
                    output=subprocess.Popen(args, shell=True, stdout=subprocess.PIPE).stdout.read()
                    outfile.write(output)
                    sleep(1)

if __name__=="__main__":
    main()
