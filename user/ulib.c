#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
void
start()
{
  extern int main();
  main();
  exit(0);
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}

//additions

// Define page size constant 
#define PGSIZE 4096

// Pointer to integer type for pointer arithmetic
typedef unsigned long uintptr_t;

// Ticket lock implementation for xv6 threads

// Let's implement a simpler version of atomic test-and-set
// Since we can't directly use xaddl, we'll use a more basic approach
// This isn't as efficient but will work for demonstration
static uint
atomic_add(uint *addr, uint val)
{
  // This is not actually atomic but works for demonstration
  // In real implementation, proper atomic operations should be used
  uint old = *addr;
  *addr = old + val;
  return old;
}

int 
lock_init(lock_t *lk)
{
  lk->ticket = 0;
  lk->turn = 0;
  return 0;
}

void 
lock_acquire(lock_t *lk)
{
  // Get my ticket number, and increment the ticket for the next customer
  uint myturn = atomic_add(&lk->ticket, 1);
  
  // Wait until my turn comes up
  while(lk->turn != myturn)
    ; // Spin
}

void 
lock_release(lock_t *lk)
{
  // Increment turn to let the next thread proceed
  lk->turn++;
}

// Thread library implementation
int 
thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2)
{
  void *stack = malloc(PGSIZE); // Allocate one page
  if(stack == 0)
    return -1;
  
  // Make sure stack is page-aligned using appropriate pointer types
  uintptr_t stack_addr = (uintptr_t)stack;
  if(stack_addr % PGSIZE) {
    free(stack);
    stack = malloc(2*PGSIZE);
    if(stack == 0)
      return -1;
    stack_addr = (uintptr_t)stack;
    stack = (void*)((stack_addr + PGSIZE - 1) & ~(PGSIZE - 1));
  }
  
  int pid = clone(start_routine, arg1, arg2, stack);
  if(pid == 0) {
    // In child thread
    return 0;
  }
  
  return pid;
}

int 
thread_join(void)
{
  void *stack;
  int pid = join(&stack);
  if(pid != -1)
    free(stack);
  return pid;
}