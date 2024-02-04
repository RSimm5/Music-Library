/// TODO Update File Header
///
/// Assignment details and provided code are created and
/// owned by Adam T Koehler, PhD - Copyright 2023.
/// University of Illinois Chicago - CS 251 Spring 2023

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cctype>
#include <map>
#include <set>

using namespace std;

// INSERTS GIVEN HELPERS CODE BELOW - DO NOT REMOVE
#include "helpers.cpp"
// INSERTS GIVEN HELPERS CODE ABOVE - DO NOT REMOVE

//
// NOTE TO STUDENTS:
// You may use the functions in the helpers file,
// but you may not modify the file or its contents in any way whatsoever.
//

//
// STUDENT CODE MAY GO BELOW THIS LINE
//

struct album{
    set<string> artists;
    set<string> music;
};

//Resets the music library to an empty state.
void clearCommand(map<string, album> &albums, map<string, set<string>> &artists){
    albums.clear();
    artists.clear();
    return;
}

//Exports the music library to the provided file.
//If no filename is present, musicdatabase.txt is used.
void exportCommand(string filename, const map<string, album> &albums){
    if (filename == ""){
        filename = "musicdatabase.txt";
    }
    ofstream file;
    file.open (filename);
    for(const auto &x : albums){
        file << x.first << endl;
        for(const string &y : x.second.artists){
            file << y << endl;
        }
        for(const string &y : x.second.music){
            file << y << endl;
        }
        file << x.first << endl;
    }
    file.close();
}


//processing for loadfile. Loads albums, songs, and artists into their respective containers in the music library.
void processLoad(string current_album, string line, map<string, album> &albums, map<string, set<string>> &artists, map<string, set<string>> &songs){
    album temp_album;
    set<string> temp;

    if(line.size() > 0 && isdigit(line.at(0))){
        if(albums.find(current_album) == albums.end()){
            albums.insert(pair<string, album>(current_album, temp_album)); 
        }
        albums.at(current_album).music.insert(line);

        if(songs.find(line) == songs.end()){
            songs.insert(pair<string, set<string>>(line, temp));
        }
        songs.at(line).insert(current_album);
    }
    
    else if(line.size() > 0){
        if(artists.find(line) == artists.end()){
            artists.insert(pair<string, set<string>>(line, temp));
        }
        artists.at(line).insert(current_album);
        albums.at(current_album).artists.insert(line);
    }
}

// Appends the file contents into the music library.
// This command either adds to an empty or existing library.
// If no filename is present, musicdatabase.txt is used.
void loadCommand(string filename, map<string, album> &albums, map<string, set<string>> &artists, map<string, set<string>> &songs){
    album temp_album;
    string line, current_album;

    if(filename == ""){
        filename = "musicdatabase.txt";
    }
    ifstream file;
    file.open(filename);
    if(!file.is_open()){
        cout << "Error: Could not open music library file - " << filename << endl;
        return;
    }

    getline(file, current_album);
    albums.insert(pair<string, album>(current_album, temp_album));

    while(getline(file, line)){
        if(current_album == line){
            getline(file, current_album);
            albums.insert(pair<string, album>(current_album, temp_album));
            continue;
        }
        processLoad(current_album, line, albums, artists, songs);
        
    }
    albums.erase("");
    file.close();
}

// dumps the entire music library to the console.
void dumpStats(const map<string, album> &albums){
    cout << "Your Current Music Library Includes" << endl 
         << "===================================" << endl;
    for(const auto &x : albums){
        cout << x.first << endl;
        for(const string &y : x.second.artists){
            cout << " " << y << endl;
        }
        for(const string &y : x.second.music){
            cout << "   " << y << endl;
        }
    }
    cout << endl;
}

// Displays the totals for the loaded music library. The
// total albums, number of unique artists, and number of 
// songs (song duplicates may exist). Adding the -d option
void statsCommand(string dump, const map<string, album> &albums, const map<string, set<string>> &artists){
    int total_records = 0, total_artists = 0, total_songs = 0;
    for(const auto &x : albums){
        total_records++;
        for(const string &y : x.second.music){
            total_songs++;
        }
    }
    for(const auto &x: artists){
        total_artists++;
    }
    cout << "Overall Music Library Stats" << endl
         << "===========================" << endl
         << "Total Records: " << total_records << endl
         << "Total Unique Artists: " << total_artists << endl
         << "Total Songs: " << total_songs << endl << endl;
    if(dump == "-d"){
        dumpStats(albums);
    }
    return;
}

//splits string separated by space. Makes each term lowercase for comparison.
void splitString(string line, string &type, set<string> &items ) {
    stringstream sstream(line);
    sstream >> type;
    while (sstream.good()) {
        string item;
        getline(sstream, item, ' ');
        tolower(item);
        items.insert(item);
    }
    items.erase("");
}

//checks if string conains terms, dont have - terms, and has + terms
bool searchValidity(const set<string> &terms, string term){
    bool has_term = 0;
    for(const string &t : terms){
        if(t.substr(0,1) == "+" && term.find(t.substr(1,t.size())) == string::npos){ //if does not contain +, invalid
            return false;
        }
        else if(t.substr(0,1) == "-" && term.find(t.substr(1,t.size())) != string::npos){ //if contains -, invalid
            return false;
        }
        else if(term.find(t) != string::npos ||
                t.substr(0,1) == "+" && term.find(t.substr(1,t.size())) != string::npos){
            has_term = 1;
        }
    }
    return has_term;
}

//searches for terms in albums 
void searchAlbums(const set<string> &terms, const map<string, album> &albums, set<string> &found){
    for(const auto &x : albums){
        string m = x.first;
        tolower(m);
        int counter = 0;
        if(searchValidity(terms, m)){
            found.insert(x.first);
        }
    }
}

//searches for terms in artists
void searchArtists(set<string> &terms, const map<string, set<string>> &artists, set<string> &found){
    for(const auto &x : artists){
        string m = x.first;
        tolower(m);
        int counter = 0;
        if(searchValidity(terms, m)){
            for(string f : x.second){
                found.insert(f);
            }
        }
    }
}
void searchSongs(const set<string> &terms, const map<string, set<string>> &songs, set<string> &found){
    for(const auto &x : songs){
        string m = x.first;
        tolower(m);
        int counter = 0;
        if(searchValidity(terms, m)){
            for(string f : x.second){
                found.insert(f);
            }
        }
    }
}


// search terms separated into a list.
// search terms are not case sensitive
// +: set intersection.  -: Difference.
// search [type] [term] [additional terms]
void searchCommand(string line, const map<string, album> &albums, const map<string, set<string>> &artists, const map<string, set<string>> &songs){
    string type, term;
    set<string> terms;
    set<string> found;

    if(line == ""){
        cout << "Error: Search terms cannot be empty." << endl;
    }
    splitString(line, type, terms);
    if(type == "album"){
        searchAlbums(terms, albums, found);
    }
    else if(type == "artist"){
        searchArtists(terms, artists, found);
    }
    else if(type == "song"){
        searchSongs(terms, songs, found);
    }

    if(found.size() == 0){
        cout << "No results found.\n\n";
        return;
    }
    cout << "Your search results exist in the following albums: \n";
    for(const string &x : found){
        cout << x << endl;
    }
    cout << endl;
}



int main()
{
    string userEntry;
    string command, remains;

    // TODO: Declare any necessary variables for the music libary
    map<string, album> albums;
    map<string, set<string>> artists, songs;

    // Display welcome message once per program execution
    cout << "Welcome to the Music Library App" << endl;
    cout << "--------------------------------" << endl;

    // Stay in menu until exit command
    do
    {
        cout << endl;
        cout << "Enter a command (help for help): " << endl;
        getline(cin, userEntry);
        cout << endl;

        // Split the user entry into two pieces
        splitFirstWord(userEntry, command, remains);
        tolower(command);

        // take an action, one per iteration, based on the command
        if (command == "help")
        {
            helpCommand();
        }
        else if (command == "clear")
        {
            clearCommand(albums, artists);
        }
        else if (command == "export")
        {
            exportCommand(remains, albums);
        }
        else if (command == "load")
        {
            loadCommand(remains, albums, artists, songs);
        }
        else if (command == "stats")
        {
            statsCommand(remains, albums, artists);
        }          
        else if (command == "search")
        {
            searchCommand(remains, albums, artists, songs);
        }

    }while(command != "exit");    

    cout << "Thank you for using the Music Library App" << endl;
    return 0;
}
