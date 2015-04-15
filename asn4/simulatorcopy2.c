#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LRU_MODE 0
#define LFU_MODE 1
#define MAX_TRACE_SIZE 128


/**
 * Data Structures
 */
typedef struct pageInfo
{
  int frameNumber;
  int timesAccessed;
  int rank; // The lower the value, the more recently used the page is
} pageInfoEntry;


int pageTableContains(pageInfoEntry* pageTable, const int pageTableCount, const int frameNum)
{
  pageInfoEntry entry;
  int i;
  for (i = 0; i < pageTableCount; i++)
  {
    entry = *(pageTable + i);
    if (entry.frameNumber == frameNum)
      return i;
  }
  return -1;
}


int getFileLineLength(const char* filename)
{
  int lines = 0;
  char c;
  FILE* fp;
  fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("Could not open file\n");
    exit(EXIT_FAILURE);
  }
  
  while (!feof(fp))
  {
    c = fgetc(fp);
    if (c == '\n')
      lines++;
  }

  return lines;
}


int* getTraceFromFile(const char* filename, int traceLength)
{
  
  // The following code of this function was used with reference to
  // http://man7.org/linux/man-pages/man3/getline.3.html
  int i = 0;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  
  int* trace;
  trace = (int*) malloc (traceLength * sizeof(int));

  // Open the file.
  FILE *fp;
  fp = fopen(filename, "r");
  
  if (fp == NULL)
  {
    printf("Could not open file.\n");
    exit(EXIT_FAILURE);
  }
  // Read the file line by line and load it into the trace storage
  for (i = 0; i < traceLength; i++)
  {
    read = getline(&line, &len, fp);
    *(trace + i) = atoi(line);
  }
  free(line);
  fclose(fp);
  return trace;
}

// Puts the page located at pageTable[index] to rank 0 and accordingly modifies the rank of all other pages.
void updatePageTableLRU(pageInfoEntry* pageTable, const int pageTableCount, const int index)
{
  // Add 1 to every page's rank when the rank is numerically lower than the one given.
  int tempRank = pageTable[index].rank;
  int i;
  for (i = 0; i < pageTableCount; i++)
  {
    if (pageTable[i].rank < tempRank)
      pageTable[i].rank++;
  }

  pageTable[index].rank = 0;
}


int findLeastRecentlyUsedPage(pageInfoEntry* pageTable, const int pageTableCount)
{
  int i;
  int highestRank = pageTable[0].rank;
  int highestIndex = 0;
  for (i = 0; i < pageTableCount; i++)
  {
    if (pageTable[i].rank > highestRank)
    {
      highestRank = pageTable[i].rank;
      highestIndex = i;
    }
  }

  return highestIndex;
}

int findLeastFrequentlyUsedPage(pageInfoEntry* pageTable, const int pageTableSize)
{
  int i;
  int lowestValue = pageTable[0].timesAccessed;
  int lowestIndex = 0;
  for (i = 0; i < pageTableSize; i++)
  {
    if (pageTable[i].timesAccessed < lowestValue)
    {
      lowestValue = pageTable[i].timesAccessed;
      lowestIndex = i;
    }
  }

  return lowestIndex;
}

int executeTraceLFU(pageInfoEntry* pageTable, const int pageTableSize, int* trace, const int traceLength)
{
  
  int i, x, frameNum, faultCount = 0, pageTableCount = 0;
  int lowest;
  
  // Go through all the traces and put them in the page table
  for(i = 0; i < traceLength; i++)
  {
    // Get the frame number we are currently on in the trace.
    frameNum = *(trace+i);
    // Check for a hit.
    x = pageTableContains(pageTable, pageTableCount, frameNum);
    if (x != -1) // Hit
    {
      pageTable[x].timesAccessed++;
    }
    // Check if the table is full
    else if (pageTableCount == pageTableSize) // Miss. The page table is full
    {
      // Evict the page with the lowest timesAccessed.
      lowest = findLeastFrequentlyUsedPage(pageTable, pageTableSize);
      pageTable[lowest].frameNumber = frameNum;
      pageTable[lowest].timesAccessed = 1;
      faultCount++;

    }
    else // Miss. The page table is not full
    {
      // Add to the end of the list
      pageTable[pageTableCount].frameNumber = frameNum;
      pageTable[pageTableCount].timesAccessed++;
      pageTableCount++;
      faultCount++;
    }
  } 

  return faultCount++;
}

int executeTraceLRU(pageInfoEntry* pageTable, const int pageTableSize, int* trace, const int traceLength)
{
  int i, x, leastRecentlyUsed, frameNum, faultCount = 0, pageTableCount = 0;
  // Go through all the traces and put them in the page table
  for(i = 0; i < traceLength; i++)
  {
    // Get the frame number we are currently on in the trace.
    frameNum = *(trace+i);
    // Check for a hit.
    x = pageTableContains(pageTable, pageTableCount, frameNum);
    if (x != -1) // Hit
    {
      updatePageTableLRU(pageTable, pageTableCount, x);    
    }
    // Check if the table is full
    else if (pageTableCount == pageTableSize) // Miss. The page table is full
    {
      printf("tuturu\n");
      leastRecentlyUsed = findLeastRecentlyUsedPage(pageTable, pageTableCount);
      updatePageTableLRU(pageTable, pageTableSize, leastRecentlyUsed);
      pageTable[leastRecentlyUsed].frameNumber = frameNum;
      faultCount++;
    }
    else // Miss. The page table is not full
    {
      // Add to the end of the list
      pageTable[pageTableCount].frameNumber = frameNum;
      pageTable[pageTableCount].rank = pageTableCount;
      // Update the table.
      updatePageTableLRU(pageTable, pageTableCount+1, pageTableCount);
      pageTableCount++;
      faultCount++;
    }
  }

  return faultCount;
}

int main(int argc, char** argv)
{
  int mode; // LFU or LRU mode
  int* trace; // Stores all frames in the trace file
  int traceLength; //  How many frames are in the trace
  pageInfoEntry* pageTable; 
  int pageTableCount = 0; // How many frames are currently in the page table
  int faultCount = -1; // Counts the amount of page faults encountered



  if (argc != 4)
  {
    printf("Error: USAGE: SIMULATOR [NUM OF FRAMES] [MEM TRACE FILE NAME] [LRU or LFU]\n");
    exit(EXIT_FAILURE);
  }

  // Get the inputs
  int pageTableSize = atoi(argv[1]);
  char* filename = argv[2];
  char* LRUorLFU = argv[3];
  
  // Check whether to use either LRU or LFU mode.
  if (strcmp(LRUorLFU, "LRU") == 0)
    mode = LRU_MODE;
  else if (strcmp(LRUorLFU, "LFU") == 0)
    mode = LFU_MODE;
  else
  {
    printf("Must be in either LRU or LFU mode.\n");
    exit(EXIT_FAILURE);
  }


  // Get the memory traces from the file.
  traceLength = getFileLineLength(filename);
  trace = getTraceFromFile(filename, traceLength);

  // Initialize the table.
  pageTable = (pageInfoEntry*) malloc (pageTableSize * sizeof(pageInfoEntry));
  
  // Go through all the traces and put them in the page table
  if (mode == LRU_MODE)
    faultCount = executeTraceLRU(pageTable, pageTableSize, trace, traceLength);
  else if (mode == LFU_MODE)
    faultCount = executeTraceLFU(pageTable, pageTableSize, trace, traceLength);
  else
    printf("Something went wrong.\n");


  // Report the fault count.
  printf("There were %d page faults during execution.\n", faultCount);

  free(pageTable);
  free(trace);

  return 0;
}
