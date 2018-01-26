// Purpose: A Feature object is created when triggered by the rolling_hash function and holds the hash fo the feature.


#include <iostream>
#include <string>
using namespace std;



class Feature
{
private:
	unsigned char f_hash[SHA_DIGEST_LENGTH];
	long f_size;
    double f_entropy;

	void test();


public:
	string getHash();
	int getSize();
    double getEntropy();

	Feature(unsigned char fash[], long fize, double fent){
		memcpy(f_hash, fash, SHA_DIGEST_LENGTH);
		f_size = fize;
        f_entropy = fent;

		// test();																				//TEST 1
	}
	virtual ~Feature(){
		// cout << "Feature destructor called." << endl;
	}
};


//-------------------------------------------------------------------------------------------
string Feature::getHash(){
	char mdString[SHA_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)f_hash[i]);
    }
    string sHash = string(mdString);
    return sHash;
}


//-------------------------------------------------------------------------------------------
int Feature::getSize(){
	return ((int) f_size);
}


//-------------------------------------------------------------------------------------------
double Feature::getEntropy(){
	return f_entropy;
}


//-------------------------------------------------------------------------------------------
void Feature::test(){
	cout << "Size: " << f_size << endl;


    char mdString[SHA_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)f_hash[i]);
    }
    printf("%s\n", mdString);
}
