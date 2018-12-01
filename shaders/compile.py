import sys
import os

for f in os.listdir(os.curdir):
    if f.endswith('.frag') or f.endswith('.vert'):
        print(f)
        os.system('glslc {0} -o {0}.spv -I include'.format(f))