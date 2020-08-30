import os
import sys

size = int(sys.argv[2])
name = sys.argv[1]
with open(name,'wb') as fout:
    fout.write(os.urandom(size))
