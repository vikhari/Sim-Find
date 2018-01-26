// THIS FILE RUNS THE SIMFIND DATABASE CREATION. NO ARGUMENTS ARE TAKEN. PLACE ALL DATABSE FILES IN THE "db_files" SUBFOLDER.


// Needed for importing modularized SimFind functions.
#include "cpp_files/FileList.hpp"
#include "cpp_files/Hashing.hpp"
#include "cpp_files/WriteToMongo.hpp"

// Needed for MongoDB connection.
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>

extern "C"{
    // #include <sqlite3.h>                            //If using sqlite3 use this instead of the MongoDB lines above.
    // #include "c_files/src/hashing.c"
}

// Needed only for this file.
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <time.h>
using boost::filesystem::path;                      //Allows avoiding have to use scope operator for 'path'.
using namespace std;



//-------------------------------------------------------------------------------------------
int main(int argc, char* argv[]){
    cout << "START  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
    clock_t timer;
    
    
    // Connection to database.
    mongocxx::instance inst{};
    mongocxx::client client_conn{mongocxx::uri{}};  //Default connection syntax, connecting to port 27017.
    mongocxx::database db = client_conn["simdb"];   //Database and collections must be created first. Run the createSimDB script.
    mongocxx::collection coll = db["feature_list"]; //Querying is always performed through the feature_list to retrieve documents of matching features. These documents contain the associated objects as well as optional attributes.
    
    
    // Create an index for the "featHash" field in the "feature_list" collection.
    auto index_specification = document{}
    << "featHash" << "hashed" << finalize;
    coll.create_index(std::move(index_specification));


    // Iterate through files and enter into database.
    FileList *fl = new FileList("test_files");
    cout << endl << "Entering objects & features into database..." << endl;
    timer = clock();
    for (int i=0;   i < (fl->filecount);   i++){

        path tmpPath = fl->filelist[i];

        Object *o = new Object(tmpPath, "test_files/");

        HashFuncs *hf = new HashFuncs(o);

        WriteToMongo *wtm = new WriteToMongo(tmpPath, o, coll);

        cout << endl;
        o->closeFile();
        delete o;
        delete hf;
        delete wtm;
    }
    timer = clock() - timer;
    float seconds = (float) timer / CLOCKS_PER_SEC;
    cout << "Creation time: " << seconds << " seconds" << endl;
    

    delete fl;
    cout << endl << "DONE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    return 0;
}