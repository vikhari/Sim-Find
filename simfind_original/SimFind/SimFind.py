#!/usr/bin/python

import config
import hashlib
from db_connector import *
from hashing import *
from cls_object import *
import os
import cProfile





connect_db()


#sys.argv[1]
for subdir, dirs, files in os.walk("/home/vik/Documents/thesis/mysim/test_files"):
    for file in files:
        tmp = os.path.join(subdir, file)
        print tmp
        obj = Object(tmp)
        createFeatureHashesFromObject(obj)
        writeObjectInDB(obj)
        obj.kill()

close_db()


