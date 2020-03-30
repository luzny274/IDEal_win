#ifndef RANDOM_PCG32_BU
#define RANDOM_PCG32_BU

#include "../Base/base.h"
/* http://www.pcg-random.org */

void inicializace_pcg32_random_r(uint32_t initstate, uint32_t initseq);
void inicializace_pcg32_random_r_2();
int pcg32_bounds(int min, int max);

static struct{
    uint64_t state;  
    uint64_t inc;
    int rs;
    void (*seed)(uint32_t initstate, uint32_t initseq);
    void (*autoseed)();
    /* Vcetne hranic */int (*next)(int min, int max) /* Vcetne hranic */;
} Random = {
    0x853c49e6748fea9bULL,
    0xda3e39cb94b95bdbULL,
    0,
    inicializace_pcg32_random_r,
    inicializace_pcg32_random_r_2,
    /* Vcetne hranic */pcg32_bounds/* Vcetne hranic */
};

uint32_t pcg32_boundedrand_r(uint32_t bound);

uint32_t pcg32_random_r()
{
    uint64_t oldstate = Random.state;
    // Advance internal state
    Random.state = oldstate * 6364136223846793005ULL + (Random.inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void inicializace_pcg32_random_r_2(){
    if(!Random.rs){
        srand(clock() % rand());
        Random.rs = 1;
    }
    inicializace_pcg32_random_r(time(NULL), rand());
}

void inicializace_pcg32_random_r(uint32_t initstate, uint32_t initseq){
    Random.state = 0U;
    Random.inc = (initseq << 1u) | 1u;
    pcg32_random_r();
    Random.state += initstate;
    pcg32_random_r();
}

int pcg32_bounds(int min, int max)/* Vcetne hranic */
{
    if(max >= min) return((int)pcg32_boundedrand_r((uint32_t)(max - min + 1)) + min);
    else return((int)pcg32_boundedrand_r((uint32_t)(min - max + 1)) + max);
}

uint32_t pcg32_boundedrand_r(uint32_t bound)
{
    // To avoid bias, we need to make the range of the RNG a multiple of
    // bound, which we do by dropping output less than a threshold.
    // A naive scheme to calculate the threshold would be to do
    //
    //     uint32_t threshold = 0x100000000ull % bound;
    //
    // but 64-bit div/mod is slower than 32-bit div/mod (especially on
    // 32-bit platforms).  In essence, we do
    //
    //     uint32_t threshold = (0x100000000ull-bound) % bound;
    //
    // because this version will calculate the same modulus, but the LHS
    // value is less than 2^32.

    uint32_t threshold = -bound % bound;

    // Uniformity guarantees that this loop will terminate.  In practice, it
    // should usually terminate quickly; on average (assuming all bounds are
    // equally likely), 82.25% of the time, we can expect it to require just
    // one iteration.  In the worst case, someone passes a bound of 2^31 + 1
    // (i.e., 2147483649), which invalidates almost 50% of the range.  In 
    // practice, bounds are typically small and only a tiny amount of the range
    // is eliminated.
    for (;;) {
        uint32_t r = pcg32_random_r();
        if (r >= threshold)
            return r % bound;
    }
}


#endif