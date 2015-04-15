#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define LRU_MODE 0
#define LFU_MODE 1
#define MAX_TRACE_SIZE 128


/**
 * Data Structures
 */
typedef struct pageInfo
{
  int frameNumber;
  int timesAccessed = 0;
} pageInfoEntry;


int pageTableContains(pageInfoEntry* pageTable, int pageTableCount, int frameNum)
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
  // The following code of this function was used with reference to
  // http://man7.org/linux/man-pages/man3/getline.3.html
  int i = 0;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  
  FILE *fp;
  fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("Could not open file");
    exit(EXIT_FAILURE);
  }

  while ((read = getline(&line, &len, fp)) != 1)
  {
    i++;
  }
  free(line);
  fclose(fp);

  return i;
}


int* getTraceFromFile(const char* filename, int traceLength)
{
  
  // The following code of this function was used with reference to
  // http://man7.org/linux/man-pages/man3/getline.3.html
  int i;
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
  while ((read = getline(&line, &len, fp)) != 1)
  {
    *(trace + i) = atoi(line);
    i++;
  }
  free(line);
  fclose(fp);
  return trace;
}

int findLeastFrequentlyUsedPage(pageInfoEntry* pageTable, const int pageTableSize)
{
  int lowestValue = pageTable[0]->timesAccessed;
  int lowestIndex = 0;
  for (int i; i < pageTableSize; i++)
  {
    if (pageTable[i]->timesAccessed < lowestValue)
    {
      lowestValue = pageTable[i]->timesAccessed;
      lowestIndex = i;
    }
  }

  return lowestIndex;
}

int executeTraceLFU(pageInfoEntry* pageTable, int* trace, int traceLength)
{
  
  int i, x, frameNum, faultCount = 0, pageTableCount = 0;
  int lowest;
  
  // Go through all the traces and put them in the page table
  for(i = 0; i < traceLength; i++)
  {
    // Get the frame number we are currently on in the trace.
    frameNum = *(trace+i);
    // Check for a hit.
    x = pageTableContains(pageTable, pageTableCount, trace);
    if (x != -1) // Hit
    {
      pageTable[x]->timesAccessed++;
    }
    // Check if the table is full
    else if (pageTableCount == pageTableSize) // Miss. The page table is full
    {
      // Evict the page with the lowest timesAccessed.
      lowest = findLeastFrequentlyUsedPage(pageTable, pageTableSize);
      pageTable[lowest]->frameNumber = frameNum;
      pageTable[lowest]->timeAccessed = 1;
      faultCount++;

    }
    else // Miss. The page table is not full
    {
      // Add to the end of the list
      (pageTable + pageTableCount)->frameNumber = frameNum;
      pageTable[pageTableCount]->timesAccessed++;
      pageTableCount++;
      faultCount++;
    }
  } 

  return faultCount++;
}

int executeTraceLRU(pageInfoEntry* pageTable, int* trace)
{
  int i, frameNum, faultCount = 0, pageTableCount = 0;
  // Go through all the traces and put them in the page table
  for(i = 0; i < traceLength; i++)
  {
    // Get the frame number we are currently on in the trace.
    frameNum = *(trace+i);
    // Check for a hit.
    if (pageTableContains(pageTable, pageTableCount, frameNum) != -1) // Hit
    {
      (pageTable+i)->timesAccessed++;
    }
    // Check if the table is full
    else if (pageTableCount == pageTableSize) // Miss. The page table is full
    {
      
    }
    else // Miss. The page table is not full
    {
      // Add to the end of the list
      (pageTable + pageTableCount)->frameNumber = frameNum;
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
    faultCount = executeTraceLRU(pageTable, frameNum, traceLength);
  else if (mode == LFU_MODE)
    faultCount = executeTraceLFU(pageTable, frameNum, traceLength);
  else
    printf("Something went wrong.\n");


  // Report the fault count.
  printf("There were %d page faults during execution.\n", faultCount);

  return 0;
}
