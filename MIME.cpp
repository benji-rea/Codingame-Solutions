#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int N; // Number of elements which make up the association table.
    cin >> N; cin.ignore();
    int Q; // Number Q of file names to be analyzed.
    cin >> Q; cin.ignore();
    
    map<string, string> mtMap;
    map<string, string>::iterator it;
    
    for (int i = 0; i < N; i++) {
        string EXT; // file extension
        string MT; // MIME type.
        cin >> EXT >> MT; cin.ignore();
        transform(EXT.begin(), EXT.end(), EXT.begin(), ::tolower);
        mtMap[EXT] = MT;
    }
    
    string UNKNOWN = "UNKNOWN";
    
    for (int i = 0; i < Q; i++) {
        string output = UNKNOWN;
        string FNAME; // One file name per line.
        getline(cin, FNAME);
        transform(FNAME.begin(), FNAME.end(), FNAME.begin(), ::tolower);
        //
        int dot = FNAME.find_last_of(".");
        if(dot > -1){
            FNAME = FNAME.substr(dot + 1);
            it = mtMap.find(FNAME);
            if(it != mtMap.end()){
                output = it->second;
            }
        }
        
        cout << output << endl;
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    //cout << "UNKNOWN" << endl; // For each of the Q filenames, display on a line the corresponding MIME type. If there is no corresponding type, then display UNKNOWN.
}
