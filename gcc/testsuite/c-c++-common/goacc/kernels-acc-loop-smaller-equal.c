/* { dg-additional-options "-fopenacc-kernels=parloops" } as this is
   specifically testing "parloops" handling.  */
/* { dg-additional-options "-O2" } */
/* { dg-additional-options "-fdump-tree-parloops1-all" } */
/* { dg-additional-options "-fdump-tree-optimized" } */

unsigned int
foo (int n)
{
  unsigned int sum = 1;

  #pragma acc kernels loop
  for (int i = 1; i <= n; i++)
    sum += i;

  return sum;
}

/* Check that only one loop is analyzed, and that it can be parallelized.  */
/* { dg-final { scan-tree-dump-times "SUCCESS: may be parallelized" 1 "parloops1" } } */
/* { dg-final { scan-tree-dump-times "(?n)__attribute__\\(\\(oacc kernels parallelized, oacc function \\(, , \\), oacc kernels, omp target entrypoint\\)\\)" 1 "parloops1" } } */
/* { dg-final { scan-tree-dump-not "FAILED:" "parloops1" } } */

/* Check that the loop has been split off into a function.  */
/* { dg-final { scan-tree-dump-times "(?n);; Function .*foo.*\\._omp_fn\\.0" 1 "optimized" } } */
