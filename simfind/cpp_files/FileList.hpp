// The code for this file was taken from: http://www.boost.org/doc/libs/1_57_0/libs/filesystem/example/simple_ls.cpp
// Purpose: iterates through the test_files folder and obtains a list of files, held in an array.


#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <iostream>
#include <string>
namespace fs = boost::filesystem;



class FileList
{
private:
    void iterate(std::__cxx11::string location_of_files);


public:
    // fs::path filelist[120000000];                                                        //Size of NSRL (120 million can't be used as size in this prototype.)
    fs::path filelist[12000000];                                                            //Order of magnitude my 16GB RAM machine could handle.
    int filecount;


    FileList(std::string test_file_path){
        iterate(test_file_path);
    }
    virtual ~FileList(){
        // std::cout << "FileList destructor called." << std::endl;
    }
};


//-------------------------------------------------------------------------------------------
void FileList::iterate(std::__cxx11::string location_of_files){
    fs::path p(fs::current_path());
    p = fs::system_complete(location_of_files);
    int file_count = 0;
    unsigned long dir_count = 0;
    unsigned long other_count = 0;
    unsigned long err_count = 0;


    std::cout << "Finding files..." << std::endl;
    if (!fs::exists(p)){                                                                    //Check if input path exists.
        std::cout << "\nNot found: " << p << std::endl;
    }


    if (fs::is_directory(p)){
        // std::cout << "\nIn directory: " << p << "\n\n";
        fs::directory_iterator end_iter;
        for (fs::directory_iterator ditr(p);  ditr != end_iter;  ++ditr) {
            try {
                if (fs::is_directory(ditr->status()))
                {
                  // ++dir_count;
                  std::cout << ditr->path().filename() << " [directory] (not processed)\n";
                }
                else if (fs::is_regular_file(ditr->status()))
                {
                  // std::cout << ditr->path().filename() << "\n";                          //TEST
                  filelist[file_count] = ditr->path().filename();
                  ++file_count;
                }
                else
                {
                  // ++other_count;
                  // std::cout << ditr->path().filename() << " [other]\n";
                }

            }
            catch (const std::exception & ex){
                ++err_count;
                std::cout << ditr->path().filename() << " " << ex.what() << std::endl;
            }
          }
          std::cout << file_count << " files found.\n";                                     //# files output.
                    // << dir_count << " directories\n"
                    // << other_count << " others\n"
                    // << err_count << " errors\n";
        }


    else{
        std::cout << "\nFound: " << p << "\n";  
    }

        
    filecount = file_count;
}