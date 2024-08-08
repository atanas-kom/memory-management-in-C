#include <stdio.h>
#include <string.h>
#include "mymemory.h"


int main()
  {
  printf ( "shell> start\n");

  initialize() ;

  char * ptr1 = (char *) mymalloc ( 10 ) ;
  strcpy (ptr1, "this test");
  printf( "shell> content of allocated memory: %s\n", ptr1 ) ;


  char * ptr2 = (char *) mymalloc ( 10 ) ;
  strcpy (ptr2, "this test");
  printf( "shell> content of allocated memory: %s\n", ptr2 ) ;


  printmemory() ;
  printsegmenttable(segmenttable) ;

  myfree ( ptr1 ) ;

  printmemory() ;
  printsegmenttable(segmenttable) ;

  myfree ( ptr2 ) ;

  printmemory() ;
  printsegmenttable(segmenttable) ;

  printf ( "shell> end\n");

  return 0;
}