/* filesys.c
 * 
 * provides interface to virtual disk
 * 
 * u07ak19
 * Atanas Komsiyski
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "filesys.h"


diskblock_t  virtualDisk [MAXBLOCKS] ;           // define our in-memory virtual, with MAXBLOCKS blocks
fatentry_t   FAT         [MAXBLOCKS] ;           // define a file allocation table with MAXBLOCKS 16-bit entries
fatentry_t   rootDirIndex            = 0 ;       // rootDir will be set by format
direntry_t * currentDir              = NULL ;
fatentry_t   currentDirIndex         = 0 ;

/* writedisk : writes virtual disk out to physical disk
 * 
 * in: file name of stored virtual disk
 */

void writedisk ( const char * filename )
{
   printf ( "writedisk> virtualdisk[0] = %s\n", virtualDisk[0].data ) ;
   FILE * dest = fopen( filename, "w" ) ;
   if ( fwrite ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
      fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
   //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
   fclose(dest) ;
   
}

void readdisk ( const char * filename )
{
   FILE * dest = fopen( filename, "r" ) ;
   if ( fread ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
      fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
   //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
      fclose(dest) ;
}


/* the basic interface to the virtual disk
 * this moves memory around
 */

void writeblock ( diskblock_t * block, int block_address )
{
   //printf ( "writeblock> block %d = %s\n", block_address, block->data ) ;
   memmove ( virtualDisk[block_address].data, block->data, BLOCKSIZE ) ;
   //printf ( "writeblock> virtualdisk[%d] = %s / %d\n", block_address, virtualDisk[block_address].data, (int)virtualDisk[block_address].data ) ;
}


/* read and write FAT
 * 
 * please note: a FAT entry is a short, this is a 16-bit word, or 2 bytes
 *              our blocksize for the virtual disk is 1024, therefore
 *              we can store 512 FAT entries in one block
 * 
 *              how many disk blocks do we need to store the complete FAT:
 *              - our virtual disk has MAXBLOCKS blocks, which is currently 1024
 *                each block is 1024 bytes long
 *              - our FAT has MAXBLOCKS entries, which is currently 1024
 *                each FAT entry is a fatentry_t, which is currently 2 bytes
 *              - we need (MAXBLOCKS /(BLOCKSIZE / sizeof(fatentry_t))) blocks to store the
 *                FAT
 *              - each block can hold (BLOCKSIZE / sizeof(fatentry_t)) fat entries
 */


//
void copy_fat(fatentry_t *FAT)
{
  diskblock_t block;
  int index = 0;
  for(int x = 1; x <= (MAXBLOCKS /(BLOCKSIZE / sizeof(fatentry_t))); x++) {
    for(int y = 0; y < (BLOCKSIZE / sizeof(fatentry_t)); y++){
      block.fat[y] = FAT[index++];
    }
    writeblock(&block, x);
  }
}


diskblock_t emptyBlock(){
  diskblock_t empty;
  for (int i = 0; i < BLOCKSIZE; i++){
  empty.data[i] = '\0';
  }
  return empty;
}

/* implement format()
 */
/////////////////////////////////////////////////////////////////////////////
void format ( )
{
   diskblock_t block ;
   direntry_t  rootDir ;
   int         pos             = 0 ;
   int         fatentry        = 0 ;
   int         fatblocksneeded =  (MAXBLOCKS / FATENTRYCOUNT ) ;

   /* prepare block 0 : fill it with '\0',
    * use strcpy() to copy some text to it for test purposes
	* write block 0 to virtual disk
	*/
  block = emptyBlock();
  
  strcpy(block.data, "3026: Operating Systems");
  writeblock(&block, 0);

	/* prepare FAT table
	 * write FAT blocks to virtual disk
	 */
  
  FAT[0] = ENDOFCHAIN;
  
  FAT[1] = 2;
  FAT[2] = ENDOFCHAIN;
  
  FAT[3] = ENDOFCHAIN;
  
  for (int i = 4; i < BLOCKSIZE; i++) {
    FAT[i] = UNUSED;}
  
  writeblock(&block, 1);
  copy_fat(FAT);
 
	 /* prepare root directory
	  * write root directory block to virtual disk
	  */

  block = emptyBlock();
  block.dir.isdir = 1;
  block.dir.nextEntry = 0;
  
  writeblock(&block, 3);
  
  rootDirIndex = 3;
  currentDirIndex = 3; 

}

/////////////////////////////////////////////////////////////////////////
MyFILE * myfopen(const char * filename, const char * mode){
    int fatblocksneeded =  (MAXBLOCKS / FATENTRYCOUNT ) ;
    int fileExists = FALSE;
    int pos;
    int freepos;
    diskblock_t block = virtualDisk[rootDirIndex];

  MyFILE * file = malloc(sizeof(MyFILE));

  //detect read/write mode
  if (strcmp(mode, "w") == 0  || strcmp(mode, "r") == 0 ){

    for(int i = 0; i < DIRENTRYCOUNT; i++){
      if (block.dir.entrylist[i].unused){ continue;}
      int comp = strcmp(block.dir.entrylist[i].name, filename);
      if( comp == 0 ){
              fileExists = TRUE;
              pos = i;
              break;
       }
    }
  //check if file exists
     if(fileExists) {
       file -> blockno = block.dir.entrylist[pos].firstblock;
       file -> pos = 0;
     }
  //if file doesn't exist create one
     else{
       if(strcmp(mode, "r") == 0 ){
         printf("File not found");
         return FALSE;
       }

       //find a free positiom
       for (int i = 0; i < DIRENTRYCOUNT; i++){
               if(block.dir.entrylist[i].unused){
                   freepos = i;
                   break;
               }
       }

       //find free fat position
       for (pos = 4; pos < MAXBLOCKS; pos++){
         if(FAT[pos] == UNUSED) break; 
       }
       FAT[pos] = ENDOFCHAIN;

       copy_fat(FAT);

       strcpy(block.dir.entrylist[freepos].name, filename);
       block.dir.entrylist[freepos].firstblock = pos;
       block.dir.entrylist[freepos].unused = FALSE; 

       writeblock(&block,rootDirIndex);
       file->blockno = pos;
       file->buffer = virtualDisk[pos];
     }

    strcpy(file->mode, mode);
    file->pos = 1;

    return file;
  }
  else{
    printf("File mode not recognised\n");
    return FALSE;
  }
}

/////////////////////////////////////////////////////////////////////////
void myfputc(int b, MyFILE * stream){
  int pos;
  int fatblocksneeded = (MAXBLOCKS / FATENTRYCOUNT);
  
  diskblock_t * buffer = &stream -> buffer;
  buffer -> data[stream -> pos] = b;
  stream -> pos++;
  
  if(stream -> pos >= BLOCKSIZE){
    writeblock(&stream -> buffer, pos);
  }
  copy_fat(fatblocksneeded);
}

/////////////////////////////////////////////////////////////////////////
int myfgetc ( MyFILE * stream ){
}

/////////////////////////////////////////////////////////////////////////
void myfclose(MyFILE * stream) {
  free(stream);
  printf("closing\n");
}





/* use this for testing
 */

void printBlock ( int blockIndex )
{
   printf ( "virtualdisk[%d] = %s\n", blockIndex, virtualDisk[blockIndex].data ) ;
}

