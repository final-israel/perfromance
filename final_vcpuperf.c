#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>

#define STR(x) #x

#define ITERATIONS 30
#define NANO_TO_MICRO 1000
#define FIB_N 10000 * 5
#define SLEEP_DURATION_SEC 1

struct lookup { 
       char *name; 
       unsigned int value;
       unsigned int avg_no_sleep;
       unsigned int avg_sleep;
       double std_no_sleep;
       double std_sleep;
};

unsigned int fib(unsigned int n)
{
       unsigned int a = 0, b = 1;
       unsigned int c = a + b;
       unsigned int i;

       for (i = 3; i <= n; i++) {
              a = b;
              b = c;
              c = a + b;
       }

       return c;
}

int main(int argc, char **argv)
{
       int result_before;
       int result_after;
       struct timespec tp_before;
       struct timespec tp_after;
       clockid_t clk_id;
       unsigned int i;
       unsigned int k;
       unsigned int j;
       unsigned int ret;
       struct lookup arr_measure_methods[] = {
              { STR(CLOCK_REALTIME), CLOCK_REALTIME},
              { STR(CLOCK_MONOTONIC), CLOCK_MONOTONIC},
              { STR(CLOCK_MONOTONIC_RAW), CLOCK_MONOTONIC_RAW},
              { STR(CLOCK_THREAD_CPUTIME_ID), CLOCK_THREAD_CPUTIME_ID},
              { STR(CLOCK_PROCESS_CPUTIME_ID), CLOCK_PROCESS_CPUTIME_ID},
       };
       int avg;
       int diff;
       int diffs[ITERATIONS];
       double var;
       unsigned int sum_diff;

       for (i = 0; i < (sizeof (arr_measure_methods) / sizeof (struct lookup)); i++){
              clk_id = arr_measure_methods[i].value;
              for (k = 0; k < 2; k++){
                     /* measurment code start */
                     avg = 0;
                     // Bring diffs to CPU cache
                     memset(diffs, 0, sizeof (diffs) / sizeof (int));
                     // Warmup the CPU
                     ret = fib(FIB_N * 1000);
                     for (j = 0; j < ITERATIONS; j++){
                            if (k == 1){
                                   sleep(SLEEP_DURATION_SEC);
                            }
                            result_before = clock_gettime(clk_id, &tp_before);
                            ret = fib(FIB_N + ret % 2 + tp_before.tv_nsec % 2);
                            result_after = clock_gettime(clk_id, &tp_after);
                            diff = tp_after.tv_nsec - tp_before.tv_nsec;
                            diff /= NANO_TO_MICRO;
                            avg += diff;
                            diffs[j] = diff;
                     }
                     /* measurment code end */

                     avg /= ITERATIONS;
                     sum_diff = 0;
                     for (j = 0; j < ITERATIONS; j++){
                            sum_diff += pow(diffs[j], 2);
                     }
                     var = (sum_diff / ITERATIONS) - pow(avg, 2);

                     if (k == 0){
                            arr_measure_methods[i].avg_no_sleep = avg;
                            arr_measure_methods[i].std_no_sleep = sqrt(var);
                     }
                     else{
                            arr_measure_methods[i].avg_sleep = avg;
                            arr_measure_methods[i].std_sleep = sqrt(var);
                     }
              }

              printf(
                     "Measuring method: %s\n"
                     "    No sleep fib avg: %u usec\n"
                     "    No sleep fib std: %f usec\n"
                     "    With sleep fin avg: %u usec\n"
                     "    With sleep fib std: %f usec\n",
                     arr_measure_methods[i].name, 
                     arr_measure_methods[i].avg_no_sleep,
                     arr_measure_methods[i].std_no_sleep,
                     arr_measure_methods[i].avg_sleep,
                     arr_measure_methods[i].std_sleep
              );
       }

       return 0;
}

       /*
              result = clock_gettime(clk_id, &tp);
              printf("result: %i\n", result);
              printf("tp.tv_sec: %lld\n", tp.tv_sec);
              printf("tp.tv_nsec: %ld\n", tp.tv_nsec);
       */

/*
       CLOCK_REALTIME
              System-wide clock that measures real (i.e., wall-clock) time.  Setting this clock requires appropriate privileges.  This clock is affected by discontinuous jumps in the sys‐
              tem time (e.g., if the system administrator manually changes the clock), and by the incremental adjustments performed by adjtime(3) and NTP.
       CLOCK_REALTIME_COARSE (since Linux 2.6.32; Linux-specific)
              A faster but less precise version of CLOCK_REALTIME.  Use when you need very fast, but not fine-grained timestamps.
       CLOCK_MONOTONIC
              Clock that cannot be set and represents monotonic time since some unspecified starting point.  This clock is not affected by discontinuous jumps in the system time (e.g., if
              the system administrator manually changes the clock), but is affected by the incremental adjustments performed by adjtime(3) and NTP.
       CLOCK_MONOTONIC_COARSE (since Linux 2.6.32; Linux-specific)
              A faster but less precise version of CLOCK_MONOTONIC.  Use when you need very fast, but not fine-grained timestamps.
       CLOCK_MONOTONIC
              Clock that cannot be set and represents monotonic time since some unspecified starting point.  This clock is not affected by discontinuous jumps in the system time (e.g., if
              the system administrator manually changes the clock), but is affected by the incremental adjustments performed by adjtime(3) and NTP.
       CLOCK_MONOTONIC_COARSE (since Linux 2.6.32; Linux-specific)
              A faster but less precise version of CLOCK_MONOTONIC.  Use when you need very fast, but not fine-grained timestamps.
       CLOCK_MONOTONIC_RAW (since Linux 2.6.28; Linux-specific)
              Similar to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is not subject to NTP adjustments or the incremental adjustments performed by adjtime(3).
       CLOCK_BOOTTIME (since Linux 2.6.39; Linux-specific)
              Identical  to CLOCK_MONOTONIC, except it also includes any time that the system is suspended.  This allows applications to get a suspend-aware monotonic clock without having
              to deal with the complications of CLOCK_REALTIME, which may have discontinuities if the time is changed using settimeofday(2).
       CLOCK_PROCESS_CPUTIME_ID (since Linux 2.6.12)
              Per-process CPU-time clock (measures CPU time consumed by all threads in the process).
       CLOCK_THREAD_CPUTIME_ID (since Linux 2.6.12)
              Thread-specific CPU-time clock.
 */
