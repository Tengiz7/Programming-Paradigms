#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
#define pb push_back
#define pf pop_front
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

void finalMethod(string source, string target, const imdb& db){
  list<path> partialPaths; 
  set<string> previouslySeenActors;
  set<film> previouslySeenFilms;

  path pth(source);
  partialPaths.pb(pth);
  while(1){
    if(partialPaths.empty() && partialPaths.front().getLength() <= 5){
        break;
    }
    path cur = partialPaths.front();
    partialPaths.pf();
    string str = cur.getLastPlayer();
    vector<film> films;
    db.getCredits(str, films);
    int  i = 0;
    for(; i < films.size(); ++i){
      film tmp = films[i];
      if(previouslySeenFilms.find(tmp) == previouslySeenFilms.end()){
        previouslySeenFilms.insert(tmp);
        vector<string> vec;
        db.getCast(tmp, vec);
        for(int j = 0; j < vec.size(); j++){
          string actor = vec[j];
          if(previouslySeenActors.find(actor) == previouslySeenActors.end()){
            previouslySeenActors.insert(actor);
            path dup = cur;
            dup.addConnection(tmp, actor);
            if(actor == target){
              cout << dup << endl;
              return;
            } else {
              partialPaths.pb(dup);
            }
          }
        }
      }
    }
  }
  cout << endl << "No path between those two people could be found." << endl << endl;

}

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    return 1;
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      finalMethod(source, target,db);
    }
  }
  
  cout << "Thanks for playing!" << endl;
  return 0;
}

