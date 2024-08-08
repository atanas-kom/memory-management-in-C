/* mymemory.c
 * 
 * provides interface to memory management
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mymemory.h"


// our memory
Byte        mymemory [MAXMEM] ;
Segment_t * segmenttable = NULL;


void initialize ()
{
   printf ( "initialize> start\n");
   // set memory to 0
   mymemory[MAXMEM] = '\0';

   // create segment table
   //Segment_t element;
   segmenttable = (Segment_t *) malloc(sizeof(Segment_t));
   // contains one segment description that declares the whole memory
   // as one free segment
   segmenttable -> allocated = FALSE;
   segmenttable -> start = mymemory;
   segmenttable -> size = MAXMEM;
   segmenttable -> next = NULL;
   // create a single segment descriptor
  
   // initialise the segment

   printf ( "initialize> end\n");
}

Segment_t * findSegment ( Segment_t * list, void * ptr )
{
   printf ( "findSegment> start\n");
   while(list != NULL){
     if(list -> start == ptr){
       return list;
     }
     list = list -> next;
   }
  return NULL;
}

void myfree ( void * ptr ) //gets start pointeer
{
  printf ( "myfree> start\n");
  Segment_t * exists = findSegment(segmenttable, ptr);
  exists -> allocated = FALSE;

}

void * mymalloc ( size_t size )
{
   printf ( "mymalloc> start\n");

   // implement the mymalloc functionality
  if(size == 0 || size > MAXMEM){
    return NULL;
    }
  else if(findFree(segmenttable, size) == NULL){
    return NULL;    
  }
  else {
    Segment_t * pt = findFree(segmenttable, size);
    Segment_t * new_seg = (Segment_t *) malloc(sizeof(Segment_t));
    insertAfter(pt, new_seg);
    
    new_seg -> allocated = FALSE;
    new_seg -> start = ((pt -> start) + size);
    new_seg -> size = ((pt -> size) - size);
    
    pt -> allocated = TRUE;
    pt -> size = size;
    return pt -> start;
  }
}

// void myfree ( void * ptr )
// {
//    printf ( "myfree> start\n");

// }

void mydefrag ( void ** ptrlist)
{
   printf ( "mydefrag> start\n");
   
}


// helper functions for management segmentation table
Segment_t * findFree ( Segment_t * list, size_t size )
{
   printf ( "findFree> start\n");
   while(list != NULL){
     if(list -> allocated == FALSE   &&   list -> size >= size){
       return list;
     }
     list = list -> next;
   }
  return NULL;
}

void insertAfter ( Segment_t * oldSegment, Segment_t * newSegment )
{
  //if adding at the end of the ll
  if(oldSegment == NULL){
    newSegment -> next = NULL;
    oldSegment -> next = newSegment;
  }
  //if adding in the middle of the ll
  else {
    newSegment -> next = oldSegment -> next;
    oldSegment -> next = newSegment;
  }
  
}


int isPrintable ( int c )
{
   if ( c >= 0x20 && c <= 0x7e ) return c ;

   return 0 ;
}


void printmemory ()
{
  
  printf("\n Contents of memory:");
  int i = 0;
  while (i <= MAXMEM){
    
    if(i % 10 ==0){
      printf("\n");
      printf("[%4d]", i);
    }
    
    int x = 0;
    while(x < 10){
      if(i+x<MAXMEM){
        printf("%02x ", mymemory[i+x]);
      }
      x++;
    }
    
    printf("| ");
    
    int z = 0;
    while (z < 10){
      if(i+z<MAXMEM){
        isPrintable(mymemory[i+z]) != 0 ? printf("%c ", mymemory[i+z]) : printf(". ");
      }
      z++;
    }
    
    i += 10;
  }
  printf("\n");
}



void printsegmenttable(Segment_t * table)
{
  printf("\n Segment table contents: \n");
  while(table != NULL){
    printsegmentdescriptor(table);
    table = table -> next;
    printf("\n");
  }
  
}

void printsegmentdescriptor ( Segment_t * descriptor )
{
      printf ( "\tallocated = %s\n" , (descriptor->allocated == FALSE ? "FALSE" : "TRUE" ) ) ;
      printf ( "\tstart     = %p\n" , descriptor->start ) ;
      printf ( "\tsize      = %lu\n", descriptor->size  ) ;
}

//-------------------------------------------------------------
// int main()
//   {
//   printf ( "shell> start\n");

//   initialize() ;

//   char * ptr1 = (char *) mymalloc ( 10 ) ;
//   strcpy (ptr1, "this test");
//   printf( "shell> content of allocated memory: %s\n", ptr1 ) ;


//   char * ptr2 = (char *) mymalloc ( 10 ) ;
//   strcpy (ptr2, "this test");
//   printf( "shell> content of allocated memory: %s\n", ptr2 ) ;


//   printmemory() ;
//   printsegmenttable(segmenttable) ;

//   myfree ( ptr1 ) ;

//   printmemory() ;
//   printsegmenttable(segmenttable) ;

//   myfree ( ptr2 ) ;

//   printmemory() ;
//   printsegmenttable(segmenttable) ;

//   printf ( "shell> end\n");

//   return 0;
// }