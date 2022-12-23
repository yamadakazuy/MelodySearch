//============================================================================
// Name        : stringsplit.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

vector<string> split(string& input, char delimiter) {
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
    	if (field.length() == 0)
    		continue;
        result.push_back(field);
    }
    return result;
}

int main(int argc, char *argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	string str;
	if ( argc > 1 ) {
		str = argv[1];
	} else {
		str = "*abba*abab*bba";
	}

	vector<string> p = split(str,'*');
	for(int i = 0; i < p.size(); ++i) {
		cout << p[i] << ", ";
	}
	cout << endl;
	return 0;
}
