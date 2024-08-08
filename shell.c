#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "filesys.h"

/* u07ak19
 * Atanas Komsiyski
 * 
 */

void main () 
{
  format();
  
  MyFILE * file = myfopen("testfile.txt", "w");
  
  writedisk("virtualdiskC3_C1");
  

//myfputc(EOF, file);
//myfclose(file);

}