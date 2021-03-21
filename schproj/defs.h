/*****************************************************************
*       defs.h - simplified for CPSC405 Lab by Gusty Cooper, University of Mary Washington
*       adapted from MIT xv6 by Zhiyi Huang, hzy@cs.otago.ac.nz, University of Otago
********************************************************************/

struct proc;

// proc.c
int             Exit(int);
int             Fork(int,int,int);
int             Kill(int);
void            pinit(void);
void            procdump(void);
void            scheduler(void);
int             userinit(void);
int             Wait(int);
int             Sleep(int, int);
void            Wakeup(int);
