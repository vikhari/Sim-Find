#!/usr/bin/python

import sys
import hashlib
import math
from collections import Counter
from cls_object import *
from cls_feature import *





def entropy(seq):
    p, lns = Counter(seq), float(len(seq))
    return -sum( count/lns * math.log(count/lns, 2) for count in p.values())



