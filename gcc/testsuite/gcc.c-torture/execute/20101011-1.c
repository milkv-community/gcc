/* { dg-options "-fnon-call-exceptions" } */
/* With -fnon-call-exceptions 0 / 0 should not be eliminated.  */
/* { dg-additional-options "-DSIGNAL_SUPPRESS" { target { ! signal } } } */

#ifdef SIGNAL_SUPPRESS
# define DO_TEST 0
#elif defined (__powerpc__) || defined (__PPC__) || defined (__ppc__) || defined (__POWERPC__) || defined (__ppc)
  /* On PPC division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__riscv)
  /* On RISC-V division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__SPU__)
  /* On SPU division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__sh__)
  /* On SH division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__v850__)
  /* On V850 division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__MSP430__)
  /* On MSP430 division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__RL78__)
  /* On RL78 division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__RX__)
  /* On RX division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__aarch64__)
  /* On AArch64 integer division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__TMS320C6X__)
  /* On TI C6X division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__VISIUM__)
  /* On Visium division by zero does not trap.  */
# define DO_TEST 0
#elif defined (__mips__) && !defined(__linux__)
  /* MIPS divisions do trap by default, but libgloss targets do not
     intercept the trap and raise a SIGFPE.  The same is probably
     true of other bare-metal environments, so restrict the test to
     systems that use the Linux kernel.  */
# define DO_TEST 0
#elif defined (__mips16) && defined(__linux__)
  /* Not all Linux kernels deal correctly the breakpoints generated by
     MIPS16 divisions by zero.  They show up as a SIGTRAP instead.  */
# define DO_TEST 0
#elif defined (__MICROBLAZE__)
/* We cannot rely on division by zero generating a trap. */
# define DO_TEST 0
#elif defined (__epiphany__)
  /* Epiphany does not have hardware division, and the software implementation
     has truly undefined behavior for division by 0.  */
# define DO_TEST 0
#elif defined (__m68k__) && !defined(__linux__)
  /* Attempting to trap division-by-zero in this way isn't likely to work on 
     bare-metal m68k systems.  */
# define DO_TEST 0
#elif defined (__CRIS__)
  /* No SIGFPE for CRIS integer division.  */
# define DO_TEST 0
#elif defined (__MMIX__)
/* By default we emit a sequence with DIVU, which "never signals an
   exceptional condition, even when dividing by zero".  */
# define DO_TEST 0
#elif defined (__arc__)
  /* No SIGFPE for ARC integer division.  */
# define DO_TEST 0
#elif defined (__arm__) && defined (__ARM_EABI__)
# ifdef __ARM_ARCH_EXT_IDIV__
  /* Hardware division instructions may not trap, and handle trapping
     differently anyway.  Skip the test if we have those instructions.  */
#  define DO_TEST 0
# else
#  include <signal.h>
  /* ARM division-by-zero behavior is to call a helper function, which
     can do several different things, depending on requirements.  Emulate
     the behavior of other targets here by raising SIGFPE.  */
int __attribute__((used))
__aeabi_idiv0 (int return_value)
{
  raise (SIGFPE);
  return return_value;
}
#  define DO_TEST 1
# endif
#elif defined (__nios2__)
  /* Nios II requires both hardware support and user configuration to
     raise an exception on divide by zero.  */
# define DO_TEST 0
#elif defined (__nvptx__)
/* There isn't even a signal function.  */
# define DO_TEST 0
#elif defined (__csky__)
  /* This presently doesn't raise SIGFPE even on csky-linux-gnu, much
     less bare metal.  See the implementation of __divsi3 in libgcc.  */
# define DO_TEST 0
#else
# define DO_TEST 1
#endif

extern void abort (void);
extern void exit (int);

#if DO_TEST

#include <signal.h>

void
sigfpe (int signum __attribute__ ((unused)))
{
  exit (0);
}

#endif

/* When optimizing, the compiler is smart enough to constant fold the
   static unset variables i and j to produce 0 / 0, but it can't
   eliminate the assignment to the global k.  */
static int i;
static int j;
int k __attribute__ ((used));

int
main ()
{
#if DO_TEST
  signal (SIGFPE, sigfpe);
  k = i / j;
  abort ();
#else
  exit (0);
#endif
}
