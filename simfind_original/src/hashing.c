#include "../header/hashing.h"
#include "../header/config.h"
#include "../header/fnv.h"
#include <stdio.h>
#include <openssl/md5.h>



uint32 roll_hashx(unsigned char c, uchar window[], uint32 rhData[])
{
    rhData[2] -= rhData[1];
    rhData[2] += (ROLLING_WINDOW * c);

    rhData[1] += c;
    rhData[1] -= window[rhData[0] % ROLLING_WINDOW];

    window[rhData[0] % ROLLING_WINDOW] = c;
    rhData[0]++;

    /* The original spamsum AND'ed this value with 0xFFFFFFFF which
       in theory should have no effect. This AND has been removed
       for performance (jk) */
    rhData[3] = (rhData[3] << 5); //& 0xFFFFFFFF;
    rhData[3] ^= c;

    return rhData[1] + rhData[2] + rhData[3];
}



int hashFileToFingerprint(FINGERPRINT *fingerprint, FILE *handle)
{
    unsigned long  bytes_read;   //stores the number of characters read from input file
    unsigned int   i;
    unsigned char  *byte_buffer     = NULL;
    unsigned int last_block_index = 0;
    uint64 rValue, hashvalue=0;


    /*we need these arrays for our extended rollhash function*/
    uchar* window = (uchar*) calloc (ROLLING_WINDOW,sizeof(uchar));
    uint32* rhData = (uint32*) calloc (4,sizeof(uint32));


    if((byte_buffer = (unsigned char*)malloc(sizeof(unsigned char)*fingerprint->filesize))==NULL)
        return -1;

    // bytes_read stores the number of characters we read from our file
    fseek(handle,0L, SEEK_SET);	
    if((bytes_read = fread(byte_buffer,sizeof(unsigned char),fingerprint->filesize,handle))==0)
        return -1;


    for(i=0; i<bytes_read; i++)
    {
        /*
         * rValue = djb2x(byte_buffer[i],window,i);
         */
        rValue  = roll_hashx(byte_buffer[i], window, rhData);  

        if (rValue % BLOCK_SIZE == BLOCK_SIZE-1) // || chunk_index >= BLOCK_SIZE_MAX)
        {

        	hashvalue = fnv64Bit(byte_buffer, last_block_index, i); //,current_index, FNV1_64_INIT);
        	add_hash_to_fingerprint(fingerprint, hashvalue); //printf("%i %llu \n", i, hashvalue);

        	//set indicies new
            last_block_index = i+1;
            if(i+SKIPPED_BYTES < bytes_read)
            	i += SKIPPED_BYTES;

            rhData = (uint32*) calloc (4,sizeof(uint32));
            window = (uchar*) calloc (ROLLING_WINDOW,sizeof(uchar));
        }
    }

    hashvalue = fnv64Bit(byte_buffer, last_block_index, bytes_read-1);
    add_hash_to_fingerprint(fingerprint, hashvalue);

    free(byte_buffer);
    free(rhData);
    free(window);
    return 1;	
}

int hashPacketBuffer(FINGERPRINT *fingerprint, const unsigned char *packet, const size_t length)
{
    unsigned int i;
    unsigned int last_block_index = 0;
    uint64 rValue, hashvalue=0;
    bool first = 1;

    uchar window[ROLLING_WINDOW] = {0};
    uint32 rhData[4]             = {0};


    for(i=0; i<length;i++)
    {
        rValue  = roll_hashx(packet[i], window, rhData);  

        if (rValue % BLOCK_SIZE == BLOCK_SIZE-1) 
        {

        		hashvalue = fnv64Bit(packet, last_block_index, i); //,current_index, FNV1_64_INIT);
        		add_hash_to_fingerprint(fingerprint, hashvalue);

        		last_block_index = i+1;

            if(i+SKIPPED_BYTES < length)
            	i += SKIPPED_BYTES;
        }
    }

   	hashvalue = fnv64Bit(packet, last_block_index, length-1);
   	add_hash_to_fingerprint(fingerprint, hashvalue);

    return 1;
}


void print_md5value(unsigned char *md5_value)
{
    int i;
    for(i=0;i<MD5_DIGEST_LENGTH;i++)
        printf("%02x",md5_value[i]);
    puts("");
}
