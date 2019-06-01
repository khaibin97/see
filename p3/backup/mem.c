#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
        int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */
 
/*
 * Helper function for bit-masking
 * Returns the last 2 bits of the address as an integer
 * Returns 0 for both free
 * Returns 1 for current alloc prev free
 * Returns 2 for prev alloc current free
 * Returns 3 for both alloc
 * -1 if Null address
 */
 int bitmask(blk_hdr* blk){
   if(blk != NULL){
     return (blk->size_status) & 7;
   }
     return -1;
 }
/*
 * Helper funtion for getting address without the status
 * Masks the last 2 bit away
 * Returns integer value of size of blk
 * Returns -1 if Null addr
 */
 int blksize(blk_hdr* blk){
   if(blk != NULL){
     return (blk->size_status) - bitmask(blk);
   }
   return -1;
 }
 
/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */                    
void* Alloc_Mem(int size) { 
    static int mem_size = 0;//size of the full alloc' heap
    if(mem_size == 0){
      mem_size = (first_blk -> size_status - 2);
    }

    if(size > mem_size){ //checks for abnormally large requests
      return NULL;
    }
    
    size += 4; //add header
    //round up and add padding
    int pad = size % 8;
    if(pad != 0){
      pad = 8 - pad;
    }
    size = size + pad;
    if(size > mem_size){ //if request ends up larger than size after including pad and header
      return NULL;
    }
    
    //Looking for free blk
    blk_hdr *best_fit = NULL;
    blk_hdr *curr_hdr = first_blk;
    int currSize = 0;
    while (curr_hdr -> size_status != 1){
      currSize = blksize(curr_hdr);//get current size
      
      if((bitmask(curr_hdr) == 2) || (bitmask(curr_hdr) == 0 )){ //If blk is free
        if(size <= currSize){ //size fits
         
          if(best_fit == NULL){ //first comparison, best fit uninit
            best_fit = curr_hdr;
          }
          
          if(currSize < blksize(best_fit)){ //if current blk is bigger than best fit, just skip
              best_fit = curr_hdr;
          }
        }
      }
    
      printf("currhdr:%p bestfit:%p\n",curr_hdr,best_fit);
      curr_hdr = (blk_hdr*) ((char*)curr_hdr + currSize);//next blk
    }
    
    //Allocating
    curr_hdr = best_fit; //move to best fit blk or NULL if all blk full
    currSize = blksize(best_fit);//-1 if Null
    
    if(size > currSize){ //No free space
      printf("Full - size: %d currsize: %d\n",size,currSize);Dump_Mem();                                                                                                    //dumpmem
      return NULL;
      
    }else if(size == currSize) { //Exact fit
      curr_hdr -> size_status += 1;//a-bit ON
      blk_hdr *next = (blk_hdr*)((char*)curr_hdr + blksize(curr_hdr));
      if(next -> size_status != 1){
       next -> size_status += 2;//p-bit for next hdr ON
      }
      printf("Exact\n");Dump_Mem();                                                                                                    //dumpmem
      return (void*) ((char*)curr_hdr+4);//return payload
      
    }else if(size < currSize) { //Splitting
      blk_hdr *newHeader = (blk_hdr*) ((char*)curr_hdr + size); //new free Header
      int newSize = currSize - size;
      newHeader -> size_status = newSize + 2; //newSize/10
      blk_hdr *footer = (blk_hdr*) ((char*)newHeader + newSize - 4); //new Footer
      footer -> size_status = newSize;
      
      curr_hdr -> size_status = size + bitmask(curr_hdr);//new alloc' header, bitmask for p-bit if any
      curr_hdr -> size_status += 1;//a-bit ON
      printf("Splitting\n");Dump_Mem();                                                                                                    //dumpmem
      return (void*) ((char*)curr_hdr+4);//return payload
    }
    
    return NULL;
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */                    
int Free_Mem(void *ptr) {                        
    //ptr checking
    if(ptr == NULL){//NULL
      return -1;
    }else if(((int)ptr % 8) != 0){//8 byte aligned
      return -1;
    }
    
    //Compute header
    blk_hdr *curr_hdr = (blk_hdr*) ((char*)ptr - 4);
    blk_hdr *temp = NULL;
    
    //Freeing
    if(bitmask(curr_hdr) == 2 || bitmask(curr_hdr) == 0){ 
      //skip, block already free
    } else {
      //check next block
      temp = (blk_hdr*) ((char*)curr_hdr + blksize(curr_hdr));
      if(bitmask(temp) == 2 || bitmask(temp) == 0){//next block free
        curr_hdr -> size_status += blksize(temp);
      }else{
        temp -> size_status -= 2; //turn p-bit off
      }
      
      //check prev block
      if(bitmask(curr_hdr) == 1 || bitmask(curr_hdr) == 0){//prev block free
        temp = (blk_hdr*) ((char*)curr_hdr - 4);
        int footer = temp->size_status;
        temp = (blk_hdr*) ((char*)curr_hdr - footer); //temp at prev addr
        temp -> size_status += blksize(curr_hdr);
        curr_hdr = temp;
      }
      
      //no coalesce or coalesce already set up
      curr_hdr -> size_status = blksize(curr_hdr) + 2; //curr_hdr/10
      temp = (blk_hdr*) ((char*)curr_hdr + blksize(curr_hdr) - 4);//footer addr
      temp -> size_status = blksize(curr_hdr);//set footer
     
     Dump_Mem();                                                                                                    //dumpmem
      return 0;
    }
    
    return -1;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */                    
int Init_Mem(int sizeOfRegion)
{                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
     allocated_once = 1;

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */                     
void Dump_Mem() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
