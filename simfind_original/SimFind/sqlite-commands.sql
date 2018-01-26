--execute in commandline by sqlite3 db.db < sqlite-commands.sql

CREATE TABLE FeatureTable(
   featureHash	 	CHAR(40)  	PRIMARY KEY	NOT NULL,
   entropy          REAL,
   length           INT3,
   ignore           INT1,
   counter          INT2
);


CREATE TABLE ObjectTable(
   objectHash 	CHAR(40)  	PRIMARY KEY	NOT NULL,
   size			INT3
); 

--add foreign key later
CREATE TABLE ObjectFeatureTable(
   objectHash 		CHAR(40)  	NOT NULL,
   featureHash	 	CHAR(40) 	NOT NULL,
   PRIMARY KEY(featureHash, objectHash)
); 