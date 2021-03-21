//#include <types.h>
//#include <stat.h>
//#include <user.h>
//#include <param.h>
#include <stdlib.h>
#include <stdio.h>

/*
We allocate the array mem[] to our memory allocator when morecore is called.
morecore normally asks the OS for memory using sbrk, malloc, etc.

By allocating mem[], I can instrument the code to print addresses and the 
corresponding mem[i] value. This allows you to visualize the memory allocation 
within the array.
 */
unsigned char mem[4096*16]; // 4096 units * sizeof(header)
void *memp = (void *)mem;

typedef unsigned int uint;

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.
// see https://www.cs.princeton.edu/courses/archive/fall07/cos217/lectures/14Memory-2x2.pdf for charts.

typedef long Align;

/*
union of stuct s and Align x ensures allocation on 8-byte boundary
On Intel, header is 16 bytes.
An address is 8 bytes,  a uint is 4 bytes. 
Compiler allocates a hole after the uint size.
*/
union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};
/*
 +--------------+                        \
 |    s.ptr     |  points to next block  |
 +--------------+                        >-> Header
 |    s.size    |  size of bloc          |
 +--------------+                        /
 |              | -> This address returned to user
 |    data      |  space for user data
 |              |
 +--------------+
*/

typedef union header Header; // allows using Header instead of union header

static Header base;
/*
used by morecore to get the linked list algorithm going
base.ptr, freep, and prevp (local to morecore) all point to base
See morecore for initial use of base

       +---------------------+
       |                     |
       |   +--------------+  |   \
freep -+-> |     s.ptr    +--+   |
           +--------------+      >-> this is base, s.size is 0 at first
           |     s.size   |      |
           +--------------+      /
*/

static Header *freep; // points to circular linked list of free memory
/*
freep begins as NULL, we need to call morecore to establish freep
After morecore, freep is a ciculearly, singly linked list of free memory
The following loop iterates through free memory
  for(p = prevp->s.ptr; p != freep; prevp = p, p = p->s.ptr)

      +-----------------------------------------------------+
      |                                                     |
      |  +-------+    +-------+    +-------+    +-------+   |
freep +> | s.ptr | -> | s.ptr | -> | s.ptr | -> | s.ptr | --+
         +-------+    +-------+    +-------+    +-------+
         | s.size|    | s.size|    | s.size|    | s.size|
         +-------+    +-------+    +------+     +-------+
         |       |    |       |    |       |    |       |
         | data  |    | data  |    | data  |    | data  |
         |       |    |       |    |       |    |       |
         +-------+    +-------+    +-------+    +-------+
                                     ^
                                     |
                                     +-free(ap), ap points to data, hp points to blk 

size is kept in nunits. nunit is the size of a Header, which is 16 bytes on my Mac
*/

/*
my_free is initially called with the large block of free memory.
At the initial call, freep points to base. See static Header base above.
The initial block as allocated in morecore looks like the following

 +--------------+
 |     ptr      |  has the address of mem[0]
 +--------------+
 |    size      |  has 4096, which is 4096 units or 4096 bytes * sizeof(Header)
 +--------------+
 |              |< ap points to this address
 |    data      |  has a bunch of 0s
 |              |
 +--------------+

 my_free adds this block creating a two-element circular linked list.
 See comment in morecore for how my_free adds the intial block to the list.

 my_free adds the block to the linked list, collapsing blocks when possible.

 */
void print_free();
void my_free(void *ap) { // ap points to application data, not Header
  Header *hp, *p;

  hp = (Header*)ap - 1; // hp points to Header of block with user data
  printf("========== my_free Info ==========\n");
  printf("hp: %p, mem[%ld], hp->s.size: %d, ap: %p, mem[%ld]\n", hp, (void *)hp-memp, hp->s.size, ap, ap-memp);
  // find block in front of hp
  for(p = freep; !(hp > p && hp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (hp > p || hp < p->s.ptr))
      break;
  if(hp + hp->s.size == p->s.ptr){ // collapse freed block with next
    hp->s.size += p->s.ptr->s.size;
    hp->s.ptr = p->s.ptr->s.ptr;
  } else                           // add returned block to free list
    hp->s.ptr = p->s.ptr;
  if(p + p->s.size == hp){         // collapse freed block with front of circular list
    p->s.size += hp->s.size;
    p->s.ptr = hp->s.ptr;
  } else                           // add returned block to free list
    p->s.ptr = hp;
  freep = p;
  printf("\n");
  printf("free list after call my_free(%p, mem[%ld])\n", ap, ap-memp);
  print_free();
}

/*
Called when more memory is needed for malloc's block
memory used to be core memory

This function is the interface between the C runtime library the performs malloc/free
and the OS. The C runtime libray requests chunks of memory from the OS. 
The chunks are requested in (at a minumum of) 4096*sizeof(Header).
The chunks are requested using the sbrk() function.
I updated chunk requests to be calloc() or simply an address of the array mem.

After the first call to morecore, the free list looks like the following.
Note how base remains in the linked list

       +------------------------------+
       |                              |
       |   +----------+  +----------+ |
freep -+-> |  s.ptr   +->|  s.ptr   +-+
           +----------+  +----------+
           |  s.size  |  |  s.size  |
           |  4096    |  |   0      |
           +----------+  +----------+
           |  4096    |     base
           |  bytes   |
           +----------+

morecore returns freep to my_malloc.
my_malloc carves space from the first block. See my_malloc comments for how this is done
*/
static Header* morecore(uint nu) {
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  //freemem = sbrk(nunits * sizeof(Header)); // get block from OS - original call
  //p = malloc(nu * sizeof(Header)); // get block from OS - works for this example
  //p = calloc(nu, sizeof(Header)); // get block from OS - works for this example

  p = (char *)&mem[0];

  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  printf("========== morecore Info ==========\n");
  printf("hp: %p, hp->s.size: %d, mem[%ld]\n", hp, hp->s.size, (void *)hp-memp);
  printf("adding initial block to free list\n");
  my_free((void*)(hp + 1)); // add this big block to our free list
  printf("Back in morecore after adding initial block to free list\n");
  printf("p: %p\n", p);
  printf("freep: %p, freep->s.size: %d, mem[%ld]\n", freep, freep->s.size, (void *)freep-memp);
  printf("freep+nu: %p, mem[%ld]\n", freep+nu, (void *)(freep+nu)-memp);
  printf("\n");
  return freep; 
}

/*
Prints the block given to a use call to my_malloc.
The block has a header, but the application pointer does not point to the header.
 */
void print_ublock(void *ap) { // ap points to user data, not Header
  Header *hp, *p;
  hp = (Header*)ap - 1; // hp points to Header of block with user data
  printf("========== User Block Info, header ptr and user data ptr ==========\n");
  printf("ublock: hp: %p, hp->s.ptr %p, mem[%ld], hp->s.size %d ap: %p, mem[%ld]\n", hp, hp->s.ptr, (void *)hp-memp, p->s.size, ap, (void *)ap-memp);
}

/*
Prints the circular linked list of free memory for analysis purposes
 */
void print_free() {
  printf("========== printing free mem list Info ==========\n");
  Header *p, *prevp;
  prevp = freep;
  for(p = prevp->s.ptr; p != freep; prevp = p, p = p->s.ptr)
    printf("p: %p, mem[%ld], p->s.ptr %p, mem[%ld], p->s.size %d\n", p, (void *)p-memp, p->s.ptr, (void *)p->s.ptr-memp, p->s.size);
  printf("p: %p, mem[%ld], p->s.ptr %p, mem[%ld], p->s.size %d\n", p, (void *)p-memp, p->s.ptr, (void *)p->s.ptr-memp, p->s.size);
  printf("\n");
}

/*
User asks for N bytes.
my_malloc allocates memory in terms of units.
A unit is the sizeof(Header), which is 16 bytes.
The user's N bytes is converted to a number of units that can contain N bytes.
See algorithm for converting N bytes to units below.
Essentially, you need 1 unit for the header and however many units to contain the users bytes

my_malloc traverses the freep list looking for block large enough for N bytes.
When the block is to large, it is split into two blocks where the second block (highest mem)
is allocated, and the first block (lowest mem) remains in the free list

my_malloc calls morecore when more free memory is needed.
In this example code, morecore is called on the first user call to my_malloc
 */
void* my_malloc(uint nbytes) {
  Header *p, *prevp;
  uint nunits; // # of Header's needed to hold user's bytes + header

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
//  2    = (  3    +      16        - 1)/   16          + 1;
// 1 unit for header, and 1 unit for 3 bytes
// 33    = ( 500   +      16        - 1)/   16          + 1;
// 1 unit for header and 32 units for 500 bytes
// 34    = ( 513   +      16        - 1)/   16          + 1;
// 1 unit for header and 33 units for 513 bytes

  if((prevp = freep) == 0){ // we have not allocated our big block
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr; // remove from free list
      else {
        p->s.size -= nunits;     // too big so adjust size
        p += p->s.size;          // p points to block to return, at bottom of free block
        p->s.size = nunits;      // block has nunits - amount requested
      }
      freep = prevp;             // update freep - circular linked list, can be any Header block
      return (void*)(p + 1);     // return pointer to user data
    }
    if(p == freep)               // Executed on first call to malloc and when morecore needed
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

int main() {
    printf("========== Basic Info ==========\n");
    printf("sizeof(Header): %lu\n", sizeof(Header));
    printf("&mem[0]: %p\n", mem);
    printf("&mem[4096*16-1]: %p\n", &mem[4096*16]);
    printf("&base: %p\n", &base);
    printf("\n");

    printf("First call to my_malloc results in call to morecore\n");
    printf("int *p0 = my_malloc(4);\n");
    int *p0 = my_malloc(4);
    print_ublock(p0);
    printf("p0: %p, mem[%ld]\n", p0, (void *)p0-memp);
    print_free();
    printf("my_free(p0);\n");
    my_free(p0);
    print_free();
    printf("int *p0 = my_malloc(4);\n");
    p0 = my_malloc(4);
    print_ublock(p0);
    printf("p0: %p, mem[%ld]\n", p0, (void *)p0-memp);
    print_free();
    int *p1 = my_malloc(8);
    printf("int *p1 = my_malloc(8);\n");
    print_ublock(p1);
    printf("p1: %p, mem[%ld]\n", p1, (void *)p1-memp);
    print_free();
    printf("int *p2 = my_malloc(64);\n");
    int *p2 = my_malloc(64);
    print_ublock(p2);
    printf("p2: %p, mem[%ld]\n", p2, (void *)p2-memp);
    print_free();
    printf("my_free(p0);\n");
    my_free(p0);
    print_free();
}

