#!/usr/bin/env python2
# -*- coding: UTF-8 -*-
# File: gen_list.py
# Date: Tue Jun 11 10:54:49 2013 +0800
# Author: Yuxin Wu <ppwwyyxxc@gmail.com>
import sys
f = open('list', 'w')
NVIEW = int(sys.argv[1])
NREPEAT = 1000
NPIC = int(sys.argv[2])

for k in xrange(NVIEW):
    for t in xrange(NREPEAT):
        for r in xrange(NPIC):
            print "{0:02}-{1:03}.png".format(k, r)
