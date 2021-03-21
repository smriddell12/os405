#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************
Macro definitions instead of magic numbers
******************************************************/

#define PAGES 256
#define PAGEFRAME_SIZE 256
#define BITS_IN_INT 32
#define PAGE_BITMAP_SIZE (PAGES / BITS_IN_INT)

/******************************************************
General discussion of VA space and PA space
******************************************************/

/*
VA space is 16 bits - 8 bits page num and 8 bits offset

 1 1 1 1 1 1 
 5 4 3 2 1 0 9 8|7 6 5 4 3 2 1 0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| | | | | | | | | | | | | | | | |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      VPN       |    Offset

VA is 16 bits, 
Upper 8 bits are VPN,
Lower 8 bits are offset.

PA space is 16 bits - 8 bits page frame num and 8 bits offset

 1 1 1 1 1 1 
 5 4 3 2 1 0 9 8|7 6 5 4 3 2 1 0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| | | | | | | | | | | | | | | | |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      PFN       |    Offset

PA is 16 bits
Upper 8 bits are PFN.
Lower 8 bits are offset.

A page frames are 256 bytes
We have 256 pages frames
PA space is 256*256, 65K, 65536

A page table has 256 entries - one entry for each VPN
Below defines a page table entry to be 4 bytes
Thus one page frame can hold 64 page table entries (i.e., 256/4)
You need 4 page frames to hold an entire page table.
*/

/******************************************************
Define uint type and panic() function
******************************************************/

typedef unsigned int uint;

void panic(char * s) {
    printf("panic: %s\n", s);
    exit(-1);
}

/******************************************************
Definition of memory and the cpu status register
******************************************************/

/*
main memory
mem[] is 65536 bytes (65K).
We have 256 physical page frames, and each is 256 bytes
 */
unsigned char mem[PAGES*PAGEFRAME_SIZE];

/*
cpu status reg
 */
int statusreg;

/******************************************************
OS page allocation algorithms
******************************************************/

/*
The array pagebitmap has one bit for each physical page frame.
Each element of pagebitmap is a uint, which has 32 bits.
Since we have 256 physical page frames, pagebitmap has 8 entries.
Eight uint's yields 8*32 = 256 bits, one for each page.
If a bit is 1, the page frame is allocaed.
If a bit is 0, the page is free.
Page frames 0 to 31 are in pagebitmap[0], 32 to 63 in pagebitmap[1],
and so on.
The following shows page frames 0, 2, and 32 allocated.
The blanks in bit positions are 0.

     3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8|7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[0] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1| |1|
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[1] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |1|
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[2] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[3] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[4] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[5] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[6] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
[7] | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 */

uint pagebitmap[PAGES / BITS_IN_INT];

/*
pagealloc() searches the pagebitmap for a free page frame, and returns
the first one found.
pagealloc() panics when it cannot find a free page frame.
 */
uint pagealloc() {
  uint m;
  for(int pi = 0; pi < PAGES; pi++) {
    m = 1 << (pi % BITS_IN_INT);
    if((pagebitmap[pi/BITS_IN_INT] & m) == 0){  // Is page free?
      pagebitmap[pi/BITS_IN_INT] |= m;  // Mark page in use.
      for (int i = 0; i < PAGEFRAME_SIZE; i++) // Zero pfn
        mem[pi*PAGEFRAME_SIZE + i] = 0;
      return pi;
    }
  }
  panic("pagealloc: out of blocks");
  return -1;
}

/*
pagefree() marks a page frame a free.
pagefree() panics if it is freeing a page frame that is not allocated.
 */
void pagefree(uint pi) {
  uint m = 1 << (pi % BITS_IN_INT);
  if((pagebitmap[pi/BITS_IN_INT] & m) == 0)
    panic("freeing free page");
  pagebitmap[pi/BITS_IN_INT] &= ~m;
}

/*
printpagebitmap() is a utility function to print the bitmap
 */
void printpagebitmap() {
    for (int pi = 0; pi < PAGE_BITMAP_SIZE; pi++)
        printf("0x%08x ", pagebitmap[pi]);
    printf("\n");
}


/******************************************************
OS page table definitions
A page table is an OS data structure
There is one page table per process
******************************************************/

struct pte {
    char pfn;         // maps a vpn to a pfn
    char valid;       // 1 valid
    char present;     // 1 present, 0 on disk
    char protectbits; // 1 read, 2 write, 3 read/write, 4 execute
};

struct pagetable {
    struct pte pagetable[PAGEFRAME_SIZE / sizeof(struct pte)];
};

/******************************************************
MMU components
1. ptbr - page table base register
   Contains PA of page table of proc RUNNING
2. tlb - translation look aside buffer
   Contains the most recent VPN to PFN translations
******************************************************/

struct pagetable *ptbr;

#define TLB_SIZE 4 // a small TLB
struct tlbe {
    char asid; // for future use
    char valid; // 1 valid
    char vpn;
    char pfn;
};

struct tlbe tlb[TLB_SIZE];

/*
invalidatetbl() marks all tlb entries as invalid
Called during a context switch
 */
void invalidatetlb() {
    for (int i = 0; i < TLB_SIZE; i++)
        tlb[i].valid = 0;
}

/*
va2pa converts a va to a pa
1. if the tlb has a translation, return addr using tlb.
   NOTE: access is not used in current implementation of tlb
2. use vpn in va as index into pagetable
   NOTE: Current implemenation only has 1 page allocated to page table.
         Thu, pagetable[X] for X > 63 does not work
3. If pte is present, valid, and access allowed
   a. Construct pa
   b. Update tlb with the translation
      NOTE: Current eviction algorithm always evicts tlb[0].
 */
int va2pa(int va, int access) {
    char vpn = (va & 0xff00) >> 8;
    char off = va & 0xff;
    int pa;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].vpn == vpn) { // hit
            pa = tlb[i].pfn << 8 | off;
            printf("TLB Hit.  VA: 0x%x, PA: 0x%x, vpn: %d, pfn: %d, off: %d\n", va, pa, vpn, tlb[i].pfn, off);
            return pa;
        }
    }
    printf("TLB Miss - Walk Page Table\n");
    if (ptbr->pagetable[vpn].valid) {
        if (!ptbr->pagetable[vpn].present) {
            printf("Page is on disk, vpn: %d, disk page pos: %d\n", vpn, ptbr->pagetable[vpn].pfn);
            return -3; // page on disk
        }
        else if (access != ptbr->pagetable[vpn].protectbits) {
            printf("Protection fault: vpn: %d\n", vpn);
            return -2; // protection fault
        }
        char pfn = ptbr->pagetable[vpn].pfn;
        char tlbupdated = 0;
        for (int i = 0; i < TLB_SIZE; i++) // 
            if (!tlb[i].valid) {
                tlb[i].valid = 1;
                tlb[i].vpn = vpn;
                tlb[i].pfn = pfn;
                tlbupdated = 1;
            }
        if (!tlbupdated) { // evict tlb[0]
            tlb[0].valid = 1;
            tlb[0].vpn = vpn;
            tlb[0].pfn = pfn;
        }
        pa = pfn << 8 | off;
        printf("TLB Miss. VA: 0x%x, PA: 0x%x, vpn: %d, pfn: %d, off: %d\n", va, pa, vpn, pfn, off);
        return pa;
    }
    else {
        printf("Seg fault: VA: 0x%x, vpn: %d, off: %d\n", va, vpn, off);
        return -1; // seg fault
    }
}

char memaccess(int va, int access) {
    int pa = va2pa(va, access);
    if (pa < 0)
        panic("Invalid VA");

    int pfn = pa>>8;
    int off = pa&0xff;
    int index = pfn*PAGEFRAME_SIZE + off;
    return mem[index];
}
/*
OS process management
 */
#define MAX_PROCS 8
 
enum procstate { UNUSED = 0, RUNNING, READY, BLOCKED,  };

struct proc {
    char procname[16];
    int pid;
    enum procstate procstate;
    int pagetablepfn; // pfn allocated to proc's pagetable
    struct pagetable *ptbr; // pa of pagetable for proc
};

struct proc procs[MAX_PROCS];

int running_proc = 0; 

void initosprocman() {
    for (int i = 0; i < MAX_PROCS; i++)
        procs[i].procstate = UNUSED;
}

void print_proc(struct proc *p) {
    printf("proc: %s\n", p->procname);
    printf("  pagetablepfn  : %d\n", p->pagetablepfn);
    for (int i = 0; i < PAGEFRAME_SIZE / sizeof(struct pte); i++) {
        if (p->ptbr->pagetable[i].valid)
            printf("  pte[%2d], vpn: %d, pfn: %d  ", i, i, p->ptbr->pagetable[i].pfn);
        else
            printf("  pte[%2d], invalid  ", i);
        if (i % 8 == 0)
            printf("\n");
    }
    printf("\n");
}

void print_proc_name(char *procname) {
    for (int i = 0; i < MAX_PROCS; i++)
        if (strcmp(procs[i].procname, procname) == 0)
            print_proc(&procs[i]);
}

void print_procs() {
    for (int i = 0; i < MAX_PROCS; i++)
        if (procs[i].procstate != UNUSED)
            print_proc(&procs[i]);
}

/*
allocate_proc() - allocates a proc and its page table 
1. finds a free stuct proc in the procs[]
2. allocates a physical page for the proc's page table
3. updates the struc proc members procname, pagetablepfn, and ptbr
4. return the index in procs of the allocated proc

NOTE: 
a. Current algorithm allocates one page frame for the page table.
b. A page table requires four page frames.
c. Thus a proc can only access VPN's 0 through 63
 */
int allocate_proc(char *procname) {
    for (int i = 1; i < MAX_PROCS; i++)
        if (procs[i].procstate == UNUSED) {
            procs[i].procstate = READY;
            strcpy(procs[i].procname, procname);
            int pfn = pagealloc(); // allocata pfn for proc pagetable
            procs[i].pagetablepfn = pfn;
            procs[i].ptbr = (struct pagetable *)&mem[pfn*PAGEFRAME_SIZE];
            // TODO Zero the pagetable
            return i;
        }
    return -1;

}

/*
load_proc_page() - loads a vpn with values in pagevalues
1. allocates a physical page for this vpn
2. updates the proc's pagetable entry for vpn
3. copies the values in pagevalues to the physical page

NOTE:
a. allocate_proc only allocates 1 page frame for the proc's pagetable
b. One page frame can map VPN's 0 through 63
c. Ensure the argument passed to parameter vpn is between 0 and 63
 */
int load_proc_page(char *procname, int vpn, char *pagevalues, int access) {
    for (int i = 0; i < MAX_PROCS; i++)
        if (strcmp(procs[i].procname, procname) == 0) {
            int pfn = pagealloc(); // allocata pfn for proc's page
            procs[i].ptbr->pagetable[vpn].pfn = pfn;
            procs[i].ptbr->pagetable[vpn].valid = 1;
            procs[i].ptbr->pagetable[vpn].present = 1;
            procs[i].ptbr->pagetable[vpn].protectbits = access;
            memcpy(&mem[pfn*PAGEFRAME_SIZE], pagevalues, 256);
            return 0;
        }
    return -1;

}

/*
print_proc_page() - untility function for pring the contents of proc's vpn
1. finds procname in the array of procs
2. If vpn is valid and present the data on the physical page is printed
 */
int print_proc_page(char *procname, int vpn) {
    for (int i = 0; i < MAX_PROCS; i++)
        if (strcmp(procs[i].procname, procname) == 0) {
            if (!procs[i].ptbr->pagetable[vpn].valid) {
                printf("Proc %s, vpn: %d invalid\n", procname, vpn);
                return -1;
            }
            else if (!procs[i].ptbr->pagetable[vpn].present) {
                printf("Proc %s, vpn: %d on disk\n", procname, vpn);
                return -1;
            }
            int pfn = procs[i].ptbr->pagetable[vpn].pfn;
            printf("Proc: %s, vpn: %d, pfn: %d, mem[%d], prot: %d\n", procname, vpn, pfn, pfn*PAGEFRAME_SIZE, procs[i].ptbr->pagetable[vpn].protectbits);
            int j = 0;
            for (int i = pfn*PAGEFRAME_SIZE; i < pfn*PAGEFRAME_SIZE+PAGEFRAME_SIZE; i++) {
                printf("%3d  ", mem[i]);
                j++;
                if (j == 16) {
                    printf("\n");
                    j = 0;
                }
            }
            printf("\n");
            return 0;
        }
    return -1;

}

void remove_proc(char *procname) {
    for (int i = 1; i < MAX_PROCS; i++) // begin with 1, cannot remove kernelproc
        if (strcmp(procs[i].procname, procname) == 0) {
            if (procs[i].procstate == RUNNING)
                running_proc = 0; // make kernelproc running
            procs[i].procstate = UNUSED;
            for (int j = 0; j < PAGES; j++) {
                pagefree(procs[i].ptbr->pagetable[j].pfn);
                //printf("free - sa: %d, len: %d\n", procs[i].mmu_regs[j].base_reg, procs[i].mmu_regs[j].bound_reg);
            }
            pagefree(procs[i].pagetablepfn);
            break;
        }
}

int dispatcher(char *procname) {
    for (int i = 0; i < MAX_PROCS; i++)
        if (strcmp(procs[i].procname, procname) == 0) {
            procs[running_proc].procstate = READY;
            procs[i].procstate = RUNNING;
            running_proc = i;
            ptbr = procs[i].ptbr;
            if (strcmp(procs[i].procname, "kernelproc") == 0)
                statusreg = 1;
            else
                statusreg = 0;
            return 0;
        }
    return -1;
}

int main() {

    struct pagetable p1 = { {{1, 1, 1 , 1}, {2, 1, 1, 2}, {3, 1, 0, 4}} };
    ptbr = &p1;
    invalidatetlb();

    printf("sizeof(pte): %lu\n", sizeof(struct pte));
    printf("%x\n", va2pa(0x005, 1));
    printf("%x\n", va2pa(0x10a, 2));
    printf("%x\n", va2pa(0x00a, 2));
    printf("%x\n", va2pa(0x20a, 4));

    allocate_proc("Gusty");
    print_proc_name("Gusty");
    allocate_proc("Coletta");
    print_proc_name("Coletta");
    printpagebitmap();
    char pagevalues[256];
    for (int i = 0; i < 256; i++)
        pagevalues[i] = i;
    load_proc_page("Gusty", 3, pagevalues, 3);
    for (int i = 0; i < 256; i++)
        pagevalues[i] += 3;
    load_proc_page("Coletta", 3, pagevalues, 3);
    for (int i = 0; i < 256; i++)
        pagevalues[i] += 3;
    load_proc_page("Gusty", 10, pagevalues, 3);
    for (int i = 0; i < 256; i++)
        pagevalues[i] += 3;
    load_proc_page("Coletta", 10, pagevalues, 3);
    for (int i = 0; i < 256; i++)
        pagevalues[i] += 20;
    load_proc_page("Gusty", 20, pagevalues, 3);
    for (int i = 0; i < 256; i++)
        pagevalues[i] += 20;
    load_proc_page("Coletta", 20, pagevalues, 3);
    print_proc_name("Gusty");
    print_proc_page("Gusty", 3);
    print_proc_page("Gusty", 10);
    print_proc_page("Gusty", 20);
    print_proc_page("Gusty", 30);
    printpagebitmap();

    dispatcher("Gusty");
    printf("running_proc: %d name: %s\n", running_proc, procs[running_proc].procname);
    char x = memaccess(0x303, 3);
    printf("Gusty: va 303, value: %d\n", x);
    x = memaccess(0x305, 3);
    printf("Gusty: va 305, value: %d\n", x);
    x = memaccess(0x306, 3);
    printf("Gusty: va 306, value: %d\n", x);
    x = memaccess(0xa03, 3);
    printf("Gusty: va a03, value: %d\n", x);
    x = memaccess(0x143f, 3);
    printf("Gusty: va 143f, value: %d\n", x);
    //x = memaccess(0x1003, 3);
    //printf("Gusty: va 1003, value: %d\n", x);
    
    dispatcher("Coletta");
    printf("running_proc: %d name: %s\n", running_proc, procs[running_proc].procname);
    x = memaccess(0x303, 3);
    printf("Coletta: va 303, value: %d\n", x);
    x = memaccess(0x305, 3);
    printf("Coletta: va 305, value: %d\n", x);
    x = memaccess(0x306, 3);
    printf("Coletta: va 306, value: %d\n", x);
    x = memaccess(0xa03, 3);
    printf("Coletta: va a03, value: %d\n", x);
    x = memaccess(0x143f, 3);
    printf("Coletta: va 143f, value: %d\n", x);
    x = memaccess(0x1003, 3);
    printf("Coletta: va 1003, value: %d\n", x);
    
}    

