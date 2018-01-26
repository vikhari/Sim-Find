#!/usr/bin/python

import sys
import hashlib
import math
from config import *
from cls_object import *


rhData = [0,0,0]
window = [0] * ROLLING_WINDOW
objectHash = hashlib.sha1()



def rolling_hash(char, pos):
    global rhData
    
    rhData[1] -= rhData[0]
    rhData[1] += (ROLLING_WINDOW * char)
    
    rhData[0] += char
    rhData[0] -= window[pos % ROLLING_WINDOW]
    
    window[pos % ROLLING_WINDOW] = char

    rhData[2] = (rhData[2] << 5) & 0xFFFFFFFF
    rhData[2] ^= char

    return rhData[0] + rhData[1] + rhData[2]


def createFeatureHashesFromObject(obj):
    global window
    last_block_index = 0
    i=0

    while i < obj.length:
        # CONVERT SIGNED TO UNSIGNED! (THIS IS AN 'ERROR' IN THE PYTHON VERSION)
        tempRHval = rolling_hash(ord(obj.byteSeq[i]), i) & 0xFFFFFFFF
        if tempRHval % BLOCKSIZE == BLOCKSIZE-1:

        
    while i < obj.length:
        if rolling_hash(ord(obj.byteSeq[i]), i) % BLOCKSIZE == BLOCKSIZE-1:
            
            chunk = obj.byteSeq[last_block_index:(i+1)]     #+1 is needed since this function only returns to i
            processBlock(chunk, obj)

            #skipp some bytes for higher performance
            last_block_index = i+1
            if (i+SKIPPED_BYTES < obj.length):
                i += int(math.floor(SKIPPED_BYTES))

            window = [0] * ROLLING_WINDOW    #reset window
        i = i + 1

    # last trigger point to end of file
    chunk = obj.byteSeq[last_block_index:(i+1)]
    processBlock(chunk, obj)


def processBlock(block, obj):
    #obj.updateObjectHash(block)
    f = Feature(block)
    obj.addFeature(f)

    




