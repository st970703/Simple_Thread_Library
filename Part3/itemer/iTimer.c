/* Example code for setting
 * up an interval timer in 
 * Linux. 
 * 
 * Based on existing text example
 *
 * Original Text Source:-
 * Advanced Linux Programming
 * by CodeSourcery
 *
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


struct sigaction sa;
struct itimerval timer;

void test_timer (int signum)
{
    static int count = 0;
    struct timeval ts;

    count += 1;
    gettimeofday(&ts, NULL);
    printf("\nts = %i\n", ts);
    printf ("%d.%06d: timer expired %d times\n", ts.tv_sec, ts.tv_usec, count);
}

void setUpTimer() {
    /* Install timer_handler as the signal handler for SIGVTALRM. */
    //memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &test_timer;
    sigaction (SIGVTALRM, &sa, NULL);

    /* Configure the timer to expire after 20 msec... */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 20000;
    /* ... and every 20 msec after that. */
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 20000;
    /* Start a virtual timer. It counts down whenever this process is
      executing. */
    setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

int main ()
{
    setUpTimer();

    /* Do busy work. */
    while(1 ==1);
}

