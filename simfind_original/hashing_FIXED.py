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

    # print rhData[0] + rhData[1] + rhData[2]
    return rhData[0] + rhData[1] + rhData[2]


def createFeatureHashesFromObject(obj):
    global window
    last_block_index = 0
    i=0
    loopNum = 0


    # while i < obj.length:
    #     if rolling_hash(ord(obj.byteSeq[i]), i) % BLOCKSIZE == BLOCKSIZE-1:



    while i < obj.length:
        # CONVERT SIGNED TO UNSIGNED! (THIS IS AN 'ERROR' IN THE PYTHON VERSION; compare with above)
        tempRHval = rolling_hash(ord(obj.byteSeq[i]), i) & 0xFFFFFFFF           #TEST 2
        # print tempRHval



        if tempRHval % BLOCKSIZE == BLOCKSIZE-1:                                #TEST 2
        
            # print "~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOOP!"
            # print "Location: ", i
            # loopNum = loopNum + 1


            chunk = obj.byteSeq[last_block_index:(i+1)]     #+1 is needed since this function only returns to i
            processBlock(chunk, obj)

            

            last_block_index = i+1
            # if (i+SKIPPED_BYTES < obj.length):
            #     i += int(math.floor(SKIPPED_BYTES))
            #skipp some bytes for higher performance
            window = [0] * ROLLING_WINDOW    #reset window

        




        # print last_block_index
        # if (i==21346):
        #     print "last block:", last_block_index

        # if (i==21347):
        #     print "\n"
        #     print "RUN 21347:"
        #     print "rolling hash:", rolling_hash(ord(obj.byteSeq[i]), i)
        #     print "window:", window
        #     print "last block:", last_block_index
        #     print "\n"

        # if (i==21348):
        #     print "RUN 21348:"
        #     print "rolling hash:", rolling_hash(ord(obj.byteSeq[i]), i)
        #     print "window:", window
        #     print "last block:", last_block_index
        # if (i==200200):
        #     print "RUN 200200:"
        #     print "rolling hash:", rolling_hash(ord(obj.byteSeq[i]), i)
        #     print "window:", window
        #     print "last block:", last_block_index
        # if (i==21350):
        #     break


        i = i + 1

    # print "\n"
    # print "Number loops: "
    # print loopNum



    # last trigger point to end of file
    chunk = obj.byteSeq[last_block_index:(i+1)]
    processBlock(chunk, obj)


def processBlock(block, obj):
    #obj.updateObjectHash(block)                # COMMENTED OUT OF ORIGINAL
    f = Feature(block)
    # print f.getHash()                       #TEST 3 & 4
    obj.addFeature(f)                       #Add feature to object list

    




