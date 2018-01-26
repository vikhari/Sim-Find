// Purpose: Create an Object object. Holds feature set, full file hash, and other attributes for entry into MongoDB.


#include <iostream>
#include <boost/filesystem/path.hpp>
#include <cstdio>
#include <string>
#include <openssl/sha.h>
#include "Feature.hpp"
using namespace std;



class Object
{
private:
	const char* name;
	FILE* byteSeq;
	unsigned char oHash[SHA_DIGEST_LENGTH];
	long length;
	int fSetCnt = 0;
	Feature* fSet[100000];																	//Max feature set = 100,000.

	void create_hash(FILE* bytestream, long sizeInBytes);
	void test();


public:
	void add_feature(Feature* feat);
	FILE* getByteSeq();
	unsigned char getByteSeqChar(int location_of_char);
	long getLength();
	int getFSetCnt();
	Feature* getFeat(int i);
	string getName();
	string getHash();
	void closeFile();


	Object(boost::filesystem::path filnam, string test_or_query_dir){
		name = filnam.c_str();                                                            	//Set name.
		

		string rootPath = test_or_query_dir;                                               	//Get file handler.
		string fullPath = rootPath + name;
		const char* pFullPath = fullPath.c_str();
		byteSeq = fopen(pFullPath, "rb");
		if (byteSeq == NULL){
				cout << "File " << name << endl;
				perror("Problem opening file.");
		}
		

		fseek(byteSeq, 0, SEEK_END);                                                      	//Set length.
		length = ftell(byteSeq);
		rewind (byteSeq);


		create_hash(byteSeq, length);                                                       //Set hash.
		

		// test();                                                                         	//TEST 1
	}
	virtual ~Object(){
		// cout << "Object destructor called." << endl;
	}
};


//-------------------------------------------------------------------------------------------
void Object::create_hash(FILE* bs, long sz){
	unsigned char buf[8192];
	SHA_CTX context;

	SHA1_Init(&context);
	try{
		for (;;){
			size_t len;

			len = fread(buf, 1, sizeof buf, bs);
			if (len == 0){
				break;
			}
			SHA1_Update(&context, buf, len);
		}
		SHA1_Final(oHash, &context);
	}
	catch (const std::exception & ex) {
		cout << "Error creating hash for: " << name << endl;
		cout << ex.what() << endl;
	}
	
}


//-------------------------------------------------------------------------------------------
void Object::add_feature(Feature* f){
	fSet[fSetCnt] = f;
	fSetCnt = fSetCnt + 1;
}


//-------------------------------------------------------------------------------------------
FILE* Object::getByteSeq(){
	return byteSeq;
}


//-------------------------------------------------------------------------------------------
unsigned char Object::getByteSeqChar(int loc){
	fseek(byteSeq,  (long)loc,  SEEK_SET);
	// cout << "L: " << loc << endl;														//TEST 2
	unsigned char castedChar = (unsigned char) getc(byteSeq);
	rewind (byteSeq);
	return castedChar;
}


//-------------------------------------------------------------------------------------------
long Object::getLength(){
	return length;
}


//-------------------------------------------------------------------------------------------
int Object::getFSetCnt(){
	return fSetCnt;
}


//-------------------------------------------------------------------------------------------
Feature* Object::getFeat(int i){
	return fSet[i];
}


//-------------------------------------------------------------------------------------------
string Object::getName(){
	string sName = string(name);
	return sName;
}


//-------------------------------------------------------------------------------------------
string Object::getHash(){
	char mdString[SHA_DIGEST_LENGTH*2+1];
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)oHash[i]);
	}
	string sHash = string(mdString);
	return sHash;
}


//-------------------------------------------------------------------------------------------
void Object::closeFile(){
	fclose(byteSeq);
}


//-------------------------------------------------------------------------------------------
void Object::test(){
	char mdString[SHA_DIGEST_LENGTH*2+1];
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)oHash[i]);
	}
	printf("Hash: %s\n", mdString);

	
	cout << "Length: " << length << endl;
	cout << endl;
}