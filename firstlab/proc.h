#define START_PID 1024
#define PC 15
#define LR 14
#define SP 13

/*
 * proc.h defines a process control block - info the OS keeps for each process
 * proc.h defines the function prototypes for the process API used in Lab 1
 * The style uses (for example) struc context instead of defining a type context
 */

typedef unsigned int uint;

/*
 * context defines the register context of processes
 */
struct context {
    uint regs[16];
};

enum procstate { UNUSED=0, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

/*
 * Per-process state - we will study many of these attributes in CPSC 405
 * Lab 1 maintains a doubly linked list of struct proc using prev and next
 */
struct proc {
    uint priority;               // process priority
    uint sz;                     // Size of process memory (bytes)
    char *kstack;                // Bottom of kernel stack for this process
    enum procstate state;        // Process state
    volatile int pid;            // Process ID
    struct proc *parent;         // Parent process
    struct context *context;     // swtch() here to run process
    void *chan;                  // If non-zero, sleeping on chan
    int killed;                  // If non-zero, have been killed
    char name[16];               // Process name (debugging)
    struct proc *prev;           // previous proc
    struct proc *next;           // next proc
};

/*
 * Function prototypes used in Lab 1
 */
char *state_to_string(enum procstate ps);
struct proc *find_proc(int pid);
void print_proc(struct proc *p);
void print_pid(int pid);
void print_procs();
struct proc *new_proc(char name[], uint priority, int ppid);
struct proc *find_proc(int pid);
bool enqueue_proc(struct proc *p);
void bootstrap();
bool kill(int pid);
int get_pid(char name[]);
int forq(char name[], int priority, int ppid);

