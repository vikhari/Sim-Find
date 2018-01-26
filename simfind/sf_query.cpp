// THIS FILE RUNS A SIMFIND QUERY. NO ARGUMENTS ARE TAKEN. PLACE ALL TEST FILES IN THE "test_files" SUBFOLDER.


//-------------------------------------------------------------------------------------------
// Needed for importing modularized SimFind functions.
#include "hpp_files/FileList.hpp"
#include "hpp_files/Hashing.hpp"
#include "hpp_files/QueryMongo.hpp"

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


// Functions for this file.
int menu();
double setThreshold();



//-------------------------------------------------------------------------------------------
int main(int argc, char* argv[]){
    cout << "START  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;


    // Connection to database.
    mongocxx::instance inst{};
    mongocxx::client client_conn{mongocxx::uri{}};  //Default connection syntax, connecting to port 27017.
    mongocxx::database db = client_conn["simdb"];   //Database and collections must be created first. Run the createSimDB script.
    mongocxx::collection coll = db["feature_list"]; //Querying is always performed through the feature_list to retrieve documents of matching features. These documents contain the associated objects as well as optional attributes.
    
    
    // Iterate through query files.
    FileList *fl = new FileList("query_files");


    
    int choice = 789;
    clock_t timer;
    double threshold;
    while (choice != 0){
        

        // Decide what query to perform on them.
        choice = menu();


        // Loop until user decides to quit.
        if (choice == 0){
            break;
        }


        // If threshold needs to be set, ask user for input.
        if (choice == 5 or choice == 6){
            threshold = setThreshold();
            if (threshold == -1){
                break;
            }
        }


        // Reprompt if user selects help.
        if (choice == 7){
            continue;
        }


        // Perform query.
        cout << endl << "Querying objects & features in database..." << endl << endl;
        timer = clock();
        for (int i=0;   i < (fl->filecount);   i++){
            
            path tmpPath = fl->filelist[i];

            Object *o = new Object(tmpPath, "query_files/");

            HashFuncs *hf = new HashFuncs(o);

            QueryMongo *qm = new QueryMongo(choice, &tmpPath, o, coll, threshold);
            
            cout << endl;
            o->closeFile();
            delete o;
            delete hf;
            delete qm;
        }
        timer = clock() - timer;
        float seconds = (float) timer / CLOCKS_PER_SEC;
        cout << "Query time: " << seconds << " seconds" << endl;
    }
    

    delete fl;
    cout << endl << "DONE  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    return 0;
}

//-------------------------------------------------------------------------------------------
int menu()
{
    int selection;

    cout << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"                                       << endl;
    cout << "Perform a query!"                                                                  << endl;
    cout << "Select from the following (least informative/fastest to most informative/slowest):"<< endl << endl;
    cout << "[0] Quit"                                                                          << endl;
    cout << "[1] (stop after 1 match - yes or no)   Anything match?"                            << endl;
    cout << "[2] (stop after 1 match - list them)   What matches?"                              << endl;
    cout << "[3] (visit all features - \% featSet)   How much match?"                           << endl;
    cout << "[4] (visit all features - list them)   What matches?"                              << endl;
    cout << "[5] (>=featSet threshold - list one)   What matches and how much?"                 << endl;
    cout << "[6] (>=featSet threshold - list all)   What matches and how much?"                 << endl;
    cout << "[7] More detailed explanation of options."                                         << endl << endl;

    cout << "Selection: ";
    cin  >> selection;


    if (selection == 7){

        cout                                                                                                            << endl << endl;

        cout << "Keep in mind that each of the options is performed for each file in the query_files folder."           << endl << endl;

        cout << "[1] Iterate through the query feature set and determine if a feature is in the database. Stop after "
        "the first match. Return 'no' if no features in the query set are found."                                       << endl << endl;

        cout << "[2] Same as [1] but if there is a match list the objects that contain the feature."                	<< endl << endl;

        cout << "[3] Iterate through the query feature set and determine if a feature is in the database. Don't stop "
        "until the entire feature set has been iterated. Report the percentage of query features found."                << endl << endl;

        cout << "[4] Same as [3] but in addition list all objects that contain each feature (including the total "
        "number of instances of the feature in the database). At the end also report number of features with instances"
        " over ten."                                                                                                    << endl << endl;


        cout << "[5] Set a threshold percentage (between 0 and 1). Iterate through the query feature set and retrieve"
        " all objects that contained each feature. Then iterate through the objects for each feature tallying the num"
        "ber of times an object occurs. Stop and report the filename of the first object that has {threshold x total "
        "number of features in query feature set} occurrences. In other words, list the first similar object."          << endl << endl;
        
        cout << "[6] Same as [5] but iterate through all features and all objects. List all similar objects."           << endl;

        cout                                                                                                            << endl << endl;

        cout << "Time (T) for each query (for each object):"                                                            << endl;
        cout << "q = total time needed to query database"                                                           << endl;
        cout << "f = time needed to query for single feature"                                                         << endl;
        cout << "t = user-specified threshold percentage (decimal)"                                                     << endl;
        cout << "n = number features in query object's feature set"                                                  << endl;
        cout << "i = time needed to iterate objects of each feature in memory"                                         << endl; 
        

        cout << "[1] T = ( f*1  <= q <= f*n)                       "                                                    << endl;
        cout << "[2] T = ( f*1  <= q <= f*n)                       "                                                    << endl;
        cout << "[3] T = (         q  = f*n)                       "                                                    << endl;
        cout << "[4] T = (         q  = f*n)                       "                                                    << endl;
        cout << "[5] T = (f*n*t <= q <= f*n) + i                   "                                                    << endl;
        cout << "[6] T = (f*n*t <= q <= f*n) + i                   "                                                    << endl << endl;               
    }

    return selection;
}


//-------------------------------------------------------------------------------------------
double setThreshold()
{
        double thresh;                                                                      //Ask user for threshold.
        cout << "Threshold (0-1): ";
        cin  >> thresh;
        if (0 >= thresh or thresh >= 1){
            cout << "Threshold must be between 0 and 1!" << endl;
            return (-1);
        }
        cout << endl;

        return thresh;
}
    
