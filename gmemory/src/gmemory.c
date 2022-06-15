
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gmemory.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct BLOCK
{
    void*  mallocAddress;   // Heap addresses

    size_t bytes;           // From Gmalloc

    char*  allocFile;       // File where allocation function called
    int    allocLine;       // Line where allocation function called

    char*  freeFile;        // File where Gfree was called
    int    freeLine;        // Line where Gfree was called

    int    isAlloced;       // If this address is still allocated

    struct BLOCK* next;     // linked list.
};

typedef struct BLOCK block_t;

static block_t* head = NULL;
static block_t* tail = NULL;

static char* badString = "???<ERROR>???";
static int printActions = 1;

static void addBlock (void* address, size_t bytes, const char* file, int line)
{
    block_t* newBlock;
    char* allocFile;
    const size_t fileLen = strlen(file);

    newBlock = malloc(sizeof(struct BLOCK));
    if (!newBlock)
    {
        if (printActions)
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "    [ERROR]: Failed to allocate HeapBlock\n");
            fprintf(stderr, "     Address --> [ 0x%p ]\n", address);
            fprintf(stderr, "     Bytes ----> [ %zu ]\n", bytes);
            fprintf(stderr, "     File -----> [ %s ]\n", file);
            fprintf(stderr, "     Line -----> [ %d ]\n\n", line);
        }
        return;
    }

    allocFile = malloc(sizeof(char) * (fileLen + 1));
    if (!allocFile)
    {
        if (printActions)
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "    [WARNING]: Failed to allocate string for file name...\n");
            fprintf(stderr, "     File -----> [ %s ]\n", file);
            fprintf(stderr, "     Line -----> [ %d ]\n", line);
        }
        newBlock->allocFile = badString;
    }
    else
    {
        newBlock->allocFile = memcpy(allocFile, file, sizeof(char) * (fileLen + 1));
    }

    newBlock->mallocAddress = address;
    newBlock->allocLine = line;
    newBlock->bytes = bytes;
    newBlock->freeFile = badString;
    newBlock->freeLine = -1;
    newBlock->isAlloced = 1;
    newBlock->next = NULL;

    if (head == NULL)
    {
        head = newBlock;
        tail = newBlock;
    }
    else
    {
        tail->next = newBlock;
        tail = tail->next;
    }

    if (printActions)
    {
        fprintf(stdout, "\n");
        fprintf(stdout, "    [ALLOCED BLOCK]\n");
        fprintf(stdout, "     Address --> [ 0x%p ]\n", address);
        fprintf(stdout, "     Bytes ----> [ %zu ]\n", bytes);
        fprintf(stdout, "     File -----> [ %s ]\n", file);
        fprintf(stdout, "     Line -----> [ %d ]\n\n", line);
    }
}

static void setFreed(void* address, const char* file, int line)
{
    block_t* finder = head;

    while (finder)
    {
        if (finder->mallocAddress == address)
            break;
        finder = finder->next;
    }

    if (finder)
    {
        const size_t fileLen = strlen(file);
        char* freeFile = malloc(sizeof(char) * (fileLen + 1));

        if (!freeFile)
        {
            if (printActions)
            {
                fprintf(stderr, "\n");
                fprintf(stderr, "    [WARNING]: Failed to allocate string for file name...\n");
                fprintf(stderr, "     File -----> [ %s ]\n", file);
                fprintf(stderr, "     Line -----> [ %d ]\n", line);
            }
            finder->freeFile = badString;
            return;
        }
        else
        {
            finder->freeFile = memcpy(freeFile, file, sizeof(char) * (fileLen + 1));
        }


        finder->freeLine = line;
        finder->isAlloced = 0;

        if (printActions)
        {
            fprintf(stdout, "\n");
            fprintf(stdout, "    [FREED BLOCK]\n");
            fprintf(stdout, "     Address ----> [ 0x%p ]\n", address);
            fprintf(stdout, "     Bytes ------> [ %zu ]\n", finder->bytes);
            fprintf(stdout, "     Free File --> [ %s ]\n", finder->freeFile);
            fprintf(stdout, "     Free Line --> [ %d ]\n", finder->freeLine);
        }
    }
}

void setPrintOut(int state)
{
    printActions = (state == SILENCE_ACTIONS) ? (SILENCE_ACTIONS) : (PRINT_ACTIONS);
}

void* Gmalloc(size_t bytes, const char* allocFile, int allocLine)
{
    void* heapAddr = malloc(bytes);
    if (!heapAddr)
    {
        if (printActions)
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "    [ERROR]: Failed to allocate in Gmalloc(size_t, ...)\n");
            fprintf(stderr, "     Bytes -> %zu\n", bytes);
            fprintf(stderr, "     File --> %s\n", allocFile);
            fprintf(stderr, "     Line --> %d\n\n", allocLine);
        }
        return NULL;
    }

    addBlock(heapAddr, bytes, allocFile, allocLine);

    return heapAddr;
}


void Gfree(void* block, const char* freeFile, int freeLine)
{
    setFreed(block, freeFile, freeLine);
    free(block);
}

void printHeap(FILE* stream)
{
    block_t* finder = head;
    size_t count = 0;

    fprintf(stream, "\n");
    fprintf(stream, "/------------------------------------------------------------------------------------------------------------------+\n");
    fprintf(stream, "|       Address      |    Bytes    |      Alloc File      | Alloc Line |       Free File      |  Free Line | Freed |\n");
    fprintf(stream, "+------------------------------------------------------------------------------------------------------------------+\n");

    while (finder)
    {
        fprintf(stream, "| 0x%016p ", finder->mallocAddress);
        fprintf(stream, "| % 11zu ", finder->bytes);

        if (strlen(finder->allocFile) > 17)
            fprintf(stream, "| ...%s ", (finder->allocFile) + (strlen(finder->allocFile) - 17));
        else
            fprintf(stream, "| % 20s ", finder->allocFile);
        
        fprintf(stream, "| % 10d ", finder->allocLine);

        if (strlen(finder->freeFile) > 17)
            fprintf(stream, "| ...%s ", (finder->freeFile) + (strlen(finder->freeFile) - 17));
        else
            fprintf(stream, "| % 20s ", finder->freeFile);

        fprintf(stream, "| % 10d ", finder->freeLine);
        fprintf(stream, "| %s |\n", (finder->isAlloced == 1) ? ("  No ") : (" Yes "));

        finder = finder->next;
        count++;
    }
    fprintf(stream, "+------------------------------------------------------------------------------------------------------------------+\n\n");
}

size_t currentlyAlloced()
{
    size_t bytes = 0;
    block_t* finder = head;

    while (finder)
    {
        bytes += (finder->isAlloced) ? (finder->bytes) : (0);
        finder = finder->next;
    }

    return bytes;
}

void G_ClearAll()
{
    // This function will free all of our allocated memory.
    block_t* finder = head;
    void* deleteMe = NULL;

    while (finder)
    {
        if (strncmp(finder->allocFile, badString, MAX(strlen(finder->allocFile), strlen(badString))) != 0)
            free(finder->allocFile);
        if (strncmp(finder->freeFile, badString, MAX(strlen(finder->freeFile), strlen(badString))) != 0)
            free(finder->freeFile);
        deleteMe = finder;
        finder = finder->next;

        free(deleteMe);
    }

    head = NULL;
    tail = NULL;
}


