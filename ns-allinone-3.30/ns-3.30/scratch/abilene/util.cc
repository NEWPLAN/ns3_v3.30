#include "util.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

/* generate poission process arrival interval */
double poission_gen_interval(double avg_rate)
{
    if (avg_rate > 0)
        return -logf(1.0 - (double)(rand() % RAND_MAX) / RAND_MAX) / avg_rate;
    else
        return 0;
}