#!/usr/bin/python

import functions
import hashlib


class Feature:

    entropy = 0.0
    hash = ''
    length = 0.0
    count = 0
    
    def __init__(self, byteSeq):
        self.hash = hashlib.sha1(byteSeq)
        self.entropy = functions.entropy(byteSeq)
        self.length = len(byteSeq)


    def getHash(self):
        return self.hash.hexdigest()

    def kill(self):
        del self



