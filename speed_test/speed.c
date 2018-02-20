
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#ifdef INVERT_4x4_SSE
#include "../src/invert4x4_sse.h"
#elif INVERT_4x4_LLVM
#include "../src/invert4x4_llvm.h"
#else
#include "../src/invert4x4_c.h"
#endif


static void print_matrix(float *m){
  printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8],  m[12]);
  printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9],  m[13]);
  printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
  printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
}

int main()
{
  struct timeval s, e;
  float inv[16];
  float m[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    1, 1, 1, 1,
  };
  // print_matrix(m);

  gettimeofday(&s, NULL);

  for(long i=0; i<10*1000*1000; i++) {
    invert4x4(m,inv);
    memcpy(m,inv,sizeof(m));
  }

  gettimeofday(&e, NULL);
  print_matrix(inv);
  printf("time = %lf\n", (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6);
}
