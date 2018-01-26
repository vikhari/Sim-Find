// Purpose: Inserts each Feature found in various objects into MongoDB as a document (each will contain a list of all objects that contain the feature).


#include <iostream>
#include <mongocxx/client.hpp>
#include <string>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
using namespace std;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;
using boost::filesystem::path;



class WriteToMongo
{
private:
	int featuresInserted = 0;
	mongocxx::stdx::optional<bsoncxx::document::value> currFeatDocValue;


	int checkIfExists(Feature*, mongocxx::collection coll_conn);
	void createDoc(Object*, Feature*, mongocxx::collection coll_conn);
	void updateDoc(Object*, Feature*, mongocxx::collection coll_conn);
	void test(mongocxx::collection coll_conn);


public:
	WriteToMongo(path filePath, Object* bjct, mongocxx::collection featColl_connection){
		
		Feature* currFeat;
		int exists;

		for (int i=0; i < bjct->getFSetCnt(); i++){
			currFeat = bjct->getFeat(i);			
			exists = checkIfExists(currFeat, featColl_connection);

			// cout << currFeat -> getHash() << endl;											//TEST 4 (print all hashes before determining whether to insert)
			
			if (exists == 0){
				createDoc(bjct, currFeat, featColl_connection);
			}
			else {
				updateDoc(bjct, currFeat, featColl_connection);
			}

		}
		// test(featColl_connection);															//TEST 1 (print all feature documents for file object via querying database)
		
		cout << featuresInserted << "\tfeatures inserted\t-\t" << filePath << endl; 
	}
    virtual ~WriteToMongo(){
        // cout << "WriteToMongo destructor called." << endl;
    }
};


//-------------------------------------------------------------------------------------------
int WriteToMongo::checkIfExists(Feature* erutaeF, mongocxx::collection featColl)
{
	try{
	    string featHash = erutaeF -> getHash();

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
void WriteToMongo::createDoc(Object* job, Feature* feech, mongocxx::collection featColl)
{
	string featHash = feech ->	getHash();
	int featSize	= feech ->	getSize();
    double featEnt  = feech ->  getEntropy();
	string objHash 	= job 	->	getHash();
	string objName 	= job 	->	getName();
	int objLength 	= job 	->	getLength();
	int objFSetCnt 	= job 	->	getFSetCnt();

	try {
		auto feechDocBuilder = bsoncxx::builder::stream::document{};
	    bsoncxx::document::value feechDocValue = feechDocBuilder
	    	<< "featHash" 		<< featHash
	    	<< "featSize"		<< featSize
	    	<< "featInstances"	<< 1														//Number of objects that have the feature.
            // << "featEntropy"    << featEnt

			<< objHash 			<< open_document											//All object info is stored via embedded objects.
			<< "Name" 			<< objName
			<< "Length" 		<< objLength
			<< "NumFeatures"	<< objFSetCnt
			<< "Count"			<< 1
			<< close_document
		
			<< finalize;
		bsoncxx::document::view feechView = feechDocValue.view();
		featColl.insert_one(feechView);
	}
	catch (exception& e) {
		cout << "Error writing new feature to database." << endl;
    	cout << e.what() << endl;
	}

	featuresInserted += 1;
}


//-------------------------------------------------------------------------------------------
void WriteToMongo::updateDoc(Object* job, Feature* feech, mongocxx::collection featColl)
{
	string featHash = feech -> getHash();
	int featInstances;
	int count;
	string objHash 	= job 	->	getHash();
	string objName 	= job 	->	getName();
	int objLength 	= job 	->	getLength();
	int objFSetCnt 	= job 	->	getFSetCnt();


	try{
	    bsoncxx::document::view featView = (*(this->currFeatDocValue)).view();				//Get previously retrieved document and convert to view.
    	bsoncxx::document::element elNOO = featView["featInstances"];
    	// std::string tmp = bsoncxx::to_string(elNumObj.type());								//TEST 2 (use to determine types of document fields, thus requiring no type guard)
	    // cout << tmp << endl;
	    // if (elNumObj.type() != bsoncxx::type::k_int32){
	    // 	cout << "Type guard check not passed for elNumObj" << endl;							
	    // }
    	featInstances = elNOO.get_int32().value;												
	    featInstances += 1;

	    if (auto elC = featView[objHash]["Count"]){											//Grab embedded object document's count if it exists.
		    count = elC.get_int32().value;
		    count += 1;
		}
	}
    catch (exception& e) {																	//If it doesn't, "unset document::element" exception is thrown. This block circumvents this.
    	count = 1;
    }


    try{
		auto objDocBuilder = bsoncxx::builder::stream::document{};
	    bsoncxx::document::value objDocValue = objDocBuilder
			<< objHash			<< open_document															
			<< "Name" 			<< objName
			<< "Length" 		<< objLength
			<< "NumFeatures"	<< objFSetCnt
			<< "Count"			<< count
			<< close_document
			<< finalize;
	    bsoncxx::document::view objView = objDocValue.view();

		featColl.update_one(
			document{} << "featHash" << featHash << finalize,
			
			document{}
			<< "$set" 			<< open_document
			<< "featInstances"	<< featInstances
			<< bsoncxx::builder::concatenate_doc{objView}
			<< close_document
			<< finalize);

		// cout << "featHash: " << featHash 		<< endl;									//Test 3 (use to see if number of objects is incremented properly and which files share features)
		// cout << "numObjects: " << featInstances 	<< endl;
		// cout << objName 							<< endl;
		// cout << "end update" 					<< endl << endl << endl;
	}
	catch (exception& e) {
    	cout << "Error updating existing feature." << endl;
    	cout << e.what() << endl;
    }
}


//-------------------------------------------------------------------------------------------
void WriteToMongo::test(mongocxx::collection featColl)
{
	mongocxx::cursor featCursor = featColl.find(document{} << finalize);
	int i = 1;
	for (auto doc : featCursor) {
		cout << endl;
		cout << "Feature #: " << i << endl;
		cout << bsoncxx::to_json(doc) << endl << endl;
		i++;
	}
}
