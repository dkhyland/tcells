#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <vector>
#include <QDebug>

using namespace std;

//Class that handles file IO. Only allows reading/writing of 1 file specified at runtime.
class FileIO{
public:
    //Creates the object that can be used to read and store data from a given fileName
    FileIO(string file_name) : file_name(file_name){}

    ~FileIO(){}

    //Writes data to the end of the file
    void writeFile(string data){
        out_file.open(file_name);
        //Write to the file
        out_file << data << endl;
        out_file.close();
    }

    string readFile(){
        in_file.open(file_name);
        string data;
        in_file >> data;
        in_file.close();
        return data;
    }

private:
    std::string file_name;
    ofstream out_file;
    ifstream in_file;
};

#endif // FILEIO_H
