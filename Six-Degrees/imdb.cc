using namespace std;
#define pb push_back
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#define loop(i,x) for(int i = 0; i < x; i++)



const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";


struct ura{
  const void* filename;
  void* type; 
};

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

int actorCmp (const void* ptr1, const void* ptr2){
  ura* keyPtr = (ura*) ptr1;
  char* str2 = (char*)(keyPtr->filename) + *(int*)ptr2;
  char* str1 = (char*)(keyPtr->type);
  return strcmp(str1,str2);
}

int movieCmp (const void* ptr1, const void* ptr2){
  ura* keyPtr_1 = (ura*) ptr1;
  film* moviePtr = (film*)keyPtr_1->type;
  film compMovie;
  char* ptr = (char*)keyPtr_1->filename + *(int*)ptr2;
  string title;
  // cout << "mamasheni";
  for(;*ptr != '\0'; ptr++){
    title += *ptr;
  }
  ptr++;
  int year = *ptr + 1900;
  compMovie.year = year;
  compMovie.title = title;
  // cout << moviePtr->title << " mamasheni " << compMovie.title << endl; 
  if(*moviePtr < compMovie){
    return -1;
  } else if (*moviePtr == compMovie){
    return 0;
  } else return 1;
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 

  int quantity = *(int*) actorFile;
  void* startPoint = (int*) actorFile + 1;
  char* strType = (char*)player.c_str();
  ura key;
  key.filename = actorFile;
  key.type = (void*) strType;
  int* offset = (int*)bsearch(&key, startPoint, quantity, sizeof(int), actorCmp);
  if(offset == NULL){
    return false;
  }

  char* curPos = (char*)actorFile + *offset;
  curPos += player.length() + 1 + (player.length() % 2 == 0);
  short numMovies = *(short*)curPos;
  curPos += 2;
  curPos += (curPos - (char*)actorFile) % 4;
  for(int i = 0; i < numMovies; i++){
    int iOffset = *(int*)curPos;
    curPos += 4;
    char* movieData = (char*)movieFile + iOffset;
    film iMovie;
    string name;
    for(;*movieData != '\0'; movieData++){
      name += *movieData;
    }
    movieData++;
    int movieYear = (int)*movieData + 1900;
    iMovie.title = name;
    iMovie.year = movieYear;
    films.pb(iMovie);
  }
  return true; 
}
bool imdb::getCast(const film& movie, vector<string>& players) const { 
  int quantity = *(int*)movieFile;
  void* startPoint = (int*)movieFile + 1;
  ura key;
  key.filename = movieFile;
  key.type = (void*)&movie;
  
  int* offset = (int*)bsearch(&key, startPoint, quantity, sizeof(int), movieCmp);
  if(offset == NULL){
    return false;
  }
  
  char* curPos = (char*)movieFile + *offset;
  curPos += movie.title.length() + 2 + (movie.title.length() % 2 != 0);
  short numActors = *(short*)curPos;
  curPos += 2;
  curPos += (curPos - (char*)movieFile) % 4;
  for(int i = 0; i < numActors; i++){
    int iOffset = *(int*)curPos;
    curPos += 4;
    char* actorData = (char*)actorFile + iOffset;
    string name;
    for(;*actorData != '\0'; actorData++){
      name += *actorData;
    }
    players.pb(name);
  }

  
  return true; 
  
  }

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
