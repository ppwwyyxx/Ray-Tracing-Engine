#!/usr/bin/env python2
# -*- coding: UTF-8 -*-
# File: gen_list.py

# Author: Yuxin Wu <ppwwyyxxc@gmail.com>
import sys
f = open('list', 'w')
NPIC = int(sys.argv[1])

for r in xrange(NPIC):
    print >> f, "{0:03}.png".format(r)
f.close()
