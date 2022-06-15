
//--------------------------------------------------------------------------------
// MIT License
// 
// Copyright(c) 2022 ryan - toddSmacked
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//--------------------------------------------------------------------------------

#ifndef __GMEMORY_C_DECL_H
#define __GMEMORY_C_DECL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
# ifdef __cplusplus
#  define NULL 0
# else
#  define NULL ((void*)0)
# endif
#endif

#include <stddef.h>
#include <stdio.h>

#define PRINT_ACTIONS (1)
#define SILENCE_ACTIONS (0)

    //--------------------------------------------------------
    // FUNCTIONS:
    //
    //  void   setPrintOut(int);
    //  void*  Gmalloc(size_t, const char*, int);
    //  void   Gfree(void*, const char*, int);
    //  void   printHeap(FILE*);
    //  size_t currentlyAlloced(void);
    //  void   G_ClearAll();
    //
    //--------------------------------------------------------

    //--------------------------------------------------------
    // Initially set to {PRINT_ACTIONS}.
    // If no standard output during allocations and frees is
    //  wanted, call this function with {SILENCE_ACTIONS}.
    //
    void setPrintOut(int state);


    //--------------------------------------------------------
    // Calls libc malloc.
    //
    void* Gmalloc(size_t bytes, const char* allocFile, int allocLine);
    #define gmalloc(bytes) Gmalloc((size_t)(bytes), __FILE__, __LINE__)


    //--------------------------------------------------------
    // Calls libc free.
    //
    void Gfree(void* block, const char* freeFile, int freeLine);
    #define gfree(block) Gfree((void*)(block), __FILE__, __LINE__)


    //--------------------------------------------------------
    // Prints information about heap addresses that have been
    //  allocated and freed through the gmalloc and gfree
    //  functions.
    //
    void printHeap(FILE* stream);


    //--------------------------------------------------------
    // Returns the total number of currently allocated bytes
    //  allocated through the gmalloc function.
    //
    size_t currentlyAlloced();


    //--------------------------------------------------------
    // Completely deallocates and removes all items on our
    //  side. This DOES NOT free any memory returned from
    //  Gmalloc.
    //
    void G_ClearAll();


#ifdef __cplusplus
}
#endif

#endif


