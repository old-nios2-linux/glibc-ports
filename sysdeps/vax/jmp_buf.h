/* Define the machine-dependent type `jmp_buf'.  Vax version.  */

typedef struct
  {
    PTR __fp;
    PTR __pc;
  } jmp_buf[1];
