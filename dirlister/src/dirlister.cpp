//============================================================================
// Name        : dirlister.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using std::cout;
using std::endl;
using std::string;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

    string path = "..\\smf";

    for (const auto & file : std::filesystem::directory_iterator(path))
        cout << file.path() << endl;

    return EXIT_SUCCESS;
}
