#!/usr/bin/python
# -*- coding: utf-8 -*-

from config import *
import sqlite3 as lite
import sys
from cls_object import *
from cls_feature import *

con = None

def connect_db():
    global con
    try:
        con = lite.connect(DB_NAME)
    
    except lite.Error, e:
        print "Error %s:" % e.args[0]
        sys.exit(1)


def close_db():
    global con
    try:
        con.close()
    except:
        print "Database connection connect not be closed"



def writeObjectInDB(obj):
    global con
    
    cur = con.cursor()


    #Inserts a new Object if Object does not exist
    stmt = "SELECT * FROM "+T_OBJECT+" WHERE objectHash=?"
    res = cur.execute(stmt, (obj.getHash(),)).fetchone()
        
    if(res == None):
        insert_stmt = ("INSERT INTO "+T_OBJECT+" VALUES (?,?)")
        data = (obj.getHash(), obj.length)
        cur.execute(insert_stmt,data)
    
        #insert all features into FeatureTable
        for key in obj.features:
            f = obj.features[key]
            insert_stmt = "SELECT counter FROM "+ T_FEATURE +" WHERE featureHash=?"
            res = cur.execute(insert_stmt, (f.getHash(),)).fetchone()
            
            #feature already in database so only update counter
            if(res != None):
                tmp = res[0] + 1
                insert_stmt = "UPDATE "+T_FEATURE+" SET counter="+str(tmp)+" WHERE featureHash=%s"
                cur.execute(insert_stmt, (f.getHash(),))
            
            #feature is not in database
            else:
                insert_stmt = ("INSERT INTO "+T_FEATURE+" VALUES (?,?,?,?,?)")
                cur.execute(insert_stmt, (f.getHash(), f.entropy, f.length, 0, 1))
            
            #finally insert in ObjectFeatureTable the relation
            insert_stmt = ("INSERT INTO "+T_OBJFEAT+" VALUES (?,?)")
            cur.execute(insert_stmt, (obj.getHash(), f.getHash()))
            f.kill()


    con.commit()





