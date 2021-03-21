/*****************************************************************
*       proc.h - simplified for CPSC405 Lab by Gusty Cooper, University of Mary Washington
*       adapted from MIT xv6 by Zhiyi Huang, hzy@cs.otago.ac.nz, University of Otago
********************************************************************/

#define NPROC        64  // maximum number of processes
#define NFILE        16  // maximum number of open files
#define CWD          64  // chars in cwd
#define PNAME        16  // chars in process name
#define PGSIZE     4096  // size of initial pages for proc
#define NCPU          8  // maximum number of CPUs


// Per-CPU state
struct cpu {
  uchar id;                    // Local APIC ID; index into cpus[] below
  struct context *scheduler;   // swtch() here to enter scheduler
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  
  // Cpu-local storage variables; see below
  struct cpu *cpu;
  struct proc *proc;           // The currently-running process.
};

struct cpu cpus[NCPU];

#define curr_cpu (&cpus[0])
#define curr_proc   (cpus[0].proc)

struct context {
  uint r4;
  uint r5;
  uint r6;
  uint r7;
  uint r8;
  uint r9;
  uint r10;
  uint r11;
  uint r12;
  uint lr;
  uint pc;
};

enum procstate { UNUSED=0, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  volatile int pid;            // Process ID
  struct proc *parent;         // Parent process
  struct context *context;     // swtch() here to run process
  int chan;                    // channel on which to sleep (int for Lab)
  int killed;                  // If non-zero, have been killed
  char ofiles[NFILE];          // Open files
  char cwd[CWD];               // Current directory
  char name[PNAME];            // Process name (debugging)
  int prio;		       // Priority nice value
  int time_to_c;	       // How long it takes the process to complete
};
