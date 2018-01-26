// Purpose: Based on user's input this file will execute a query option.



#include <iostream>
#include <mongocxx/client.hpp>
#include <string>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/stdx.hpp>
#include <math.h>
using namespace std;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;
using boost::filesystem::path;



class QueryMongo
{
private:
	int choice;
	path* fip;
	Object* job;
	mongocxx::collection featColl;
	Feature* currFeat;
	mongocxx::stdx::optional<bsoncxx::document::value> currFeatDocValue;
	map<string, int> numFoundPerObj;
	int highestCount = 0;
	string nameHighestCount;
	double thresh;
	int featInstancesMoreThanTen = 0;


	void anyFeatureOption();																//Option 1 or 2
	void featureSetOption();																//Option 3 or 4
	void thresholdPOption(); 																//Option 5 or 6
	int queryFeature(Feature* feat);														//Returns 1 for found or 0 for not found.
	void listObjects(Feature* feat);
	void incrementObjectDict(int totalFeatFound);
	void thresholdObjectDict();
	void printAllAboveThresh(int fNumThresh);
	int printFeatInstances(Feature* feat);
	void test();


public:
	QueryMongo(int menuChoice, path* filePath, Object* bjct, mongocxx::collection featColl_connection, double threshPercentage = 0){
		
		choice = menuChoice;
		fip = filePath;
		job = bjct;
		featColl = featColl_connection;
		thresh = threshPercentage;

		// int featInObj = job->getFSetCnt();													//TEST 3
		// cout << "feats in obj: " << featInObj << endl;

		if 	 	(choice == 1 or choice == 2){
			anyFeatureOption();
		}
		else if (choice == 3 or choice == 4){
			featureSetOption();
		}
		else if (choice == 5 or choice == 6){
			thresholdPOption();
		}
	}
    virtual ~QueryMongo(){
        // cout << "WriteToMongo destructor called." << endl;
    }
};


//-------------------------------------------------------------------------------------------
void QueryMongo::anyFeatureOption()
{
	try{
		int featFound = 0;


		for (int i=0; i < job->getFSetCnt(); i++){											//Query database for each feature.
			currFeat = job->getFeat(i);
			featFound = queryFeature(currFeat);
			if (featFound == 1){															//Stop after first find.
				break;
			}
		}


		if (choice == 1){																	//Choice 1 - report yes/no.
			if (featFound == 1){
				cout << "Yes!\t-\t" << *fip << endl;
			}
			else {
				cout << "No!\t-\t"  << *fip << endl;
			}
		}


		if (choice ==2){																	//Choice 2 - list objects.
			if (featFound == 1){
				cout << currFeat->getHash() << endl;
				listObjects(currFeat);
			}
			else {
				cout << "No features were found." << endl;
			}
		}
	}

	
	catch (exception& e) {
    	cout << "Error with the query." << endl;
    	cout << e.what() << endl;
    }
}


//-------------------------------------------------------------------------------------------
void QueryMongo::featureSetOption()
{
	try{
		double totalFeatFound = 0;


		for (int i=0; i < job->getFSetCnt(); i++){											//Query database for each feature.
			int featFound = 0;
			currFeat = job->getFeat(i);
			featFound = queryFeature(currFeat);
			if (featFound == 1){
				if (choice == 4){															//Choice 4 - list objects and number instances of feature.
					cout << currFeat->getHash() << "  -  " << printFeatInstances(currFeat) << endl;
					listObjects(currFeat);
				}
				totalFeatFound += 1;
			}
		}


		if (totalFeatFound > 0){															//Choice 3/4 - report percent of features found.
			double FSetCnt = job->getFSetCnt();
			double percentFound = (totalFeatFound / FSetCnt) * 100.00;
			cout << *fip << "\t-\t" << "Percent of features found: " << percentFound << "\%" << endl;
			if (choice == 4){
				cout << "Number of features with more than 10 instances: " << featInstancesMoreThanTen << endl;
			}
		}
		else {
			cout << "No features were found." << endl;
		}
	}


	catch (exception& e) {
    	cout << "Error with the query." << endl;
    	cout << e.what() << endl;
    }
}


//-------------------------------------------------------------------------------------------
void QueryMongo::thresholdPOption()
{
	try{
		double FSetCnt = job->getFSetCnt();													//Determine number of features to meet threshold.
		int fNumThresh = floor (thresh * FSetCnt);


		int totalFeatFound = 0;


		for (int i=0; i < job->getFSetCnt(); i++){											//Query database for each feature.
			int featFound = 0;
			currFeat = job->getFeat(i);
			featFound = queryFeature(currFeat);
			if (featFound == 1){
				incrementObjectDict(totalFeatFound);										//Increment occurrences of each object.
				thresholdObjectDict();														//Check if number of features to meet threshold has been met.
				
				if (choice == 5){															//Choice 5 - report first object to meet threshold.
					if (highestCount == fNumThresh){
						cout << *fip << "\t-\t" << "First similar object above threshold:\n";
						cout << nameHighestCount << endl;
						break;
					}
				}
				
				// test();																		//TEST 2 (print object counts from dictionary)
				totalFeatFound += 1;
			}
		}


		if (highestCount < fNumThresh){
			cout << "No objects were similar enough to meet the threshold." << endl;
		}
		else if (choice == 6){
			printAllAboveThresh(fNumThresh); 												//Choice 6 - report all objects that meet threshold.
		}


		// cout << "--------" << endl;															//TEST 2
		// test();
	}


	catch (exception& e) {
    	cout << "Error with the query." << endl;
    	cout << e.what() << endl;
    }
}


//-------------------------------------------------------------------------------------------
int QueryMongo::queryFeature(Feature* feat)
{
	try{
		    string featHash = feat -> getHash();

		    mongocxx::stdx::optional<bsoncxx::document::value> result_exists =
		    	featColl.find_one(document{}
		    		<< "featHash" << featHash
		    		<< finalize);
		    
		    if (result_exists) {
		    	currFeatDocValue = (*result_exists);
		    	return 1;}

		    else {return 0;}
	}
	catch (exception& e) {
    	cout << "Error querying for whether feature exists." << endl;
    	cout << e.what() << endl;
    }
}


//-------------------------------------------------------------------------------------------
void QueryMongo::listObjects(Feature* feat)
{

	bsoncxx::document::view featView = (*(this->currFeatDocValue)).view();


	// string featHash = feat -> getHash();														//TEST 1
	// cout << "Feature: " << featHash << endl;


	for (const bsoncxx::document::element& el : featView){									//For each embedded document in feature document list filename.
		if (el.type() == bsoncxx::type::k_document) {
			
			bsoncxx::document::view subDoc = el.get_document().value;

			bsoncxx::document::element elName = subDoc["Name"];
			cout << elName.get_utf8().value << endl;
	    }
	}


	cout << endl;
}


//-------------------------------------------------------------------------------------------
void QueryMongo::incrementObjectDict(int totalFeatFound)
{
	bsoncxx::document::view featView = (*(this->currFeatDocValue)).view();

	for (const bsoncxx::document::element& el : featView){									//For each embedded document in feature document get filename.
		if (el.type() == bsoncxx::type::k_document) {
			
			bsoncxx::document::view subDoc = el.get_document().value;

			bsoncxx::document::element elName = subDoc["Name"];
			string objName = elName.get_utf8().value.to_string();

			
			if (totalFeatFound == 0){														//If first found feature simply insert filename and one.
				numFoundPerObj[objName] = 1;
			}
			
			else{
				map<string, int>::iterator it = numFoundPerObj.begin();						//Otherwise look for existing entry and increment.
				while(it != numFoundPerObj.end()) {
					string filename = it->first;
					if (filename == objName){
						int count 	= it->second;
						count 	   += 1;
						numFoundPerObj[filename] = count;
					}
					it++;
				}
	    	}
	    }
	}
}


//-------------------------------------------------------------------------------------------
void QueryMongo::thresholdObjectDict()
{
	map<string, int>::iterator it = numFoundPerObj.begin();
	while(it != numFoundPerObj.end()) {
		string filename = it->first;
		int count 		= it->second;
		if (count > highestCount){
			highestCount = count;
			nameHighestCount = filename;
		}
		it++;
	}
}


//-------------------------------------------------------------------------------------------
void QueryMongo::printAllAboveThresh(int fNumThresh)
{
	cout << *fip << "\t-\t" << "All similar objects above threshold:\n";

	map<string, int>::iterator it = numFoundPerObj.begin();
	while(it != numFoundPerObj.end()) {
		string filename = it->first;
		int count 		= it->second;
		if (count >= fNumThresh){
			cout << filename << "\t\t(" << count << " features)" << endl;
		}
		it++;
	}
}


//-------------------------------------------------------------------------------------------
int QueryMongo::printFeatInstances(Feature* feat)
{
	int featInstance;

	bsoncxx::document::view featValue = (*(this->currFeatDocValue));
	bsoncxx::document::element elFeatInstances = featValue["featInstances"];
	featInstance = elFeatInstances.get_int32().value;
	if (featInstance > 10){
		featInstancesMoreThanTen += 1;
	}

	return featInstance;
}


//-------------------------------------------------------------------------------------------
void QueryMongo::test()
{
	map<string, int>::iterator it = numFoundPerObj.begin();
	while(it != numFoundPerObj.end()) {
		string filename = it->first;
		int count 		= it->second;
		cout << "\t" << filename << "\t" << count << endl;
		it++;
	}
}