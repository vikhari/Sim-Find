// Purpose: This file performs the slicing of the file using the rolling_hash, creates the Feature hashes, and appends them to the Object object's feature set list.


#include "Config.hpp"
#include "Object.hpp"
#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <set>
using namespace std;



class HashFuncs
{
private:
    uint32_t rhData[3] = {0,0,0};
    unsigned char window[ROLLING_WINDOW];
    unsigned char fHash[SHA_DIGEST_LENGTH];
    long fSize;
    
    void initWindow();
    uint32_t rolling_hash(unsigned char valOfChar, int position);
    void create_hash(unsigned char chunkSeq[], long chunkSize);
    double calc_entropy(string& chunkSeq);
    void test();


public:
    uint32_t createFeatureHashes(Object* obj);


    HashFuncs(Object* oddjob){
        initWindow();
        createFeatureHashes(oddjob);
    }
    virtual ~HashFuncs(){
        // cout << "HashFuncs destructor called." << endl;
    }
};


//-------------------------------------------------------------------------------------------
uint32_t HashFuncs::createFeatureHashes(Object* obj)
{
    int last_block_index = 0;
    int i = 0;
    int chunk_size;
    // int loopNum = 0;



    while (i < (int) obj->getLength()){                                                      //Move through file stream.
        unsigned char c = obj->getByteSeqChar(i);
        uint32_t tempRHval = rolling_hash(c, i);
        // cout << tempRHval << endl;                                                       //TEST 1



        if (tempRHval % BLOCKSIZE == BLOCKSIZE - 1) {
            
            // cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOOP!" << endl;                         //FOR TESTS
            // cout << "Location: " << i << endl;
            // loopNum = loopNum + 1;

            fseek(obj->getByteSeq(),  last_block_index,  SEEK_SET);                          //Seek to beginning of chunk.

            // unsigned char castedChar = (unsigned char) getc(obj->getByteSeq());           //TEST 2
            // cout << "test1: " << castedChar << endl;
            // fseek(obj->getByteSeq(),  -1,  SEEK_CUR);


            chunk_size = (i+1) - last_block_index;                                          //Get size of chunk.
            fSize = (long) chunk_size;


            unsigned char chunkBuf[chunk_size];                                             //Copy file stream into chunk buffer.
            for (int chunkchar=0; chunkchar < chunk_size; chunkchar++){
                unsigned char castedChar = (unsigned char) getc(obj->getByteSeq());
                chunkBuf[chunkchar] = castedChar;
            }


            create_hash(chunkBuf, fSize);
            // string tempStr(chunkBuf, chunkBuf+sizeof(chunkBuf) / sizeof(chunkBuf[0]));
            // double fEntropy = calc_entropy(tempStr);                                        //Calculate feature entropy.
            //cout << calc_entropy(tempStr) << endl;                                         
            // test();                                                                      //TEST 3


            Feature* IFfee = new Feature(fHash, fSize, fEntropy);                           //Create Feature object.
            obj->add_feature(IFfee);
            
            
            last_block_index = i+1;                                                         //Set end of current chunk to be beginning of next.
            // if ( (i + SKIPPED_BYTES) < (obj->getLength()) ){
            //     i += int(floor(SKIPPED_BYTES));
            // }
            rewind(obj->getByteSeq());                                                       //Reset position indicator after fseek.
            initWindow();                                                                   //Reset window.
        }
        


        // cout << last_block_index << endl;                                                //FOR TESTS
        
        // if (i==21347){
        //     cout << endl;
        //     cout << "RUN 21347: " << endl;
        //     cout << "rolling hash: " << rolling_hash(c, i) << endl;
        //     cout << "window: " << window << endl;
        //     cout << "last block: " << last_block_index << endl;
        //     cout << endl;
        // }
        
        // if (i==21350){
        //     break;
        // }



        i = i + 1;
    }
    // cout << endl;                                                                        //FOR TESTS
    // cout << "Number loops: " << endl;
    // cout << loopNum << endl;



    fseek(obj->getByteSeq(),  last_block_index,  SEEK_SET);                                  //Last trigger point to the end of the file.
    chunk_size = (i) - last_block_index;                                                    //Same as above except chunk_size uses 'i' instead of 'i+1'.
    fSize = (long) chunk_size;
    unsigned char chunkBuf[chunk_size];
    for (int chunkchar=0; chunkchar < chunk_size; chunkchar++){
        unsigned char castedChar = (unsigned char) getc(obj->getByteSeq());
        chunkBuf[chunkchar] = castedChar;
    }
    create_hash(chunkBuf, fSize);
    // string tempStr(chunkBuf, chunkBuf+sizeof(chunkBuf) / sizeof(chunkBuf[0]));
    // double fEntropy = calc_entropy(tempStr);
    // test();                                                                              //TEST 4
    Feature* WILfee = new Feature(fHash, fSize, fEntropy);
    obj->add_feature(WILfee);
    rewind(obj->getByteSeq());
}


//-------------------------------------------------------------------------------------------
void HashFuncs::initWindow()
{
    unsigned int i;
    for (i=0;  i < (sizeof(window)/sizeof(window[0]));  i++){
        window[i] = 0;
    }
}


//-------------------------------------------------------------------------------------------
uint32_t HashFuncs::rolling_hash(unsigned char val, int pos)
{
    rhData[1] -= rhData[0];
    rhData[1] += (ROLLING_WINDOW * val);

    rhData[0] += val;
    rhData[0] -= window[pos % ROLLING_WINDOW];

    window[pos % ROLLING_WINDOW] = val;

    rhData[2] = (rhData[2] << 5) & 0xFFFFFFFF;
    rhData[2] ^= val;

    //cout << rhData[0] + rhData[1] + rhData[2] << endl;                                    //TEST 1
    return rhData[0] + rhData[1] + rhData[2];
}


//-------------------------------------------------------------------------------------------
void HashFuncs::create_hash(unsigned char se[], long si){

    // cout << "test2: " << se[0] << endl;                                                  //TEST 2

    SHA1(se, (size_t) si, fHash);
    
}


//-------------------------------------------------------------------------------------------
double HashFuncs::calc_entropy(string& st){                                                 //Source: https://github.com/ekg/entropy
    vector<char> stvec(st.begin(), st.end());
    set<char> alphabet(stvec.begin(), stvec.end());
    vector<double> freqs;
    for (set<char>::iterator c = alphabet.begin(); c != alphabet.end(); ++c) {
        int ctr = 0;
        for (vector<char>::iterator s = stvec.begin(); s != stvec.end(); ++s) {
            if (*s == *c) {
                ++ctr;
            }
        }
        freqs.push_back((double)ctr / (double)stvec.size());
    }
    double ent = 0;
    double ln2 = log(2);
    for (vector<double>::iterator f = freqs.begin(); f != freqs.end(); ++f) {
        ent += *f * log(*f)/ln2;
    }
    ent = -ent;
    return ent;
}


//-------------------------------------------------------------------------------------------
void HashFuncs::test(){
    cout << "Size: " << fSize << endl;


    char mdString[SHA_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++){
        sprintf(&mdString[i*2], "%02x", (unsigned int)fHash[i]);
    }
    printf("%s\n", mdString);
}
