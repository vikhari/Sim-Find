#!/usr/bin/python

import hashlib
from cls_feature import *

class Object:
    

    def __init__(self, objectName):
        self.name = objectName
        self.byteSeq = open(objectName, "rb").read()
        self.length = len(self.byteSeq)
        self.hash = hashlib.sha1(self.byteSeq)
        self.features = {}

    def updateObjectHash(self, byteSeq):
        self.hash.update(byteSeq)

    def getHash(self):
        return self.hash.hexdigest()

    def getFeatureCount(self):
        return len(self.features)

    def addFeature(self, f):
        if f.getHash() in self.features:
            x = self.features[f.getHash()]
            x.count =+ 1
        else:
            self.features[f.getHash()] = f

    def kill(self):
        del self