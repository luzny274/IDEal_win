#ifndef BU_TIMER_CPP
#define BU_TIMER_CPP

#include "../Base/base.hpp"
#include "String.cpp"

namespace ulm{
    class ScopeTimer{
        private:
            String text;
            clock_t start;

        public:
            ScopeTimer(){
                start = clock();
            }
            ScopeTimer(String arg_text){ 
                text = arg_text; 
                start = clock();
            }
            ScopeTimer(const char * arg_text){ 
                text = arg_text;
                start = clock();
            }

            ~ScopeTimer(){
                clock_t diff = clock() - start;
                float mil = (float)((double)(diff) * (double)1000/ (double)CLOCKS_PER_SEC);
                double sec = ((double)(diff)/ (double)( CLOCKS_PER_SEC));
                printf("%s\n\tticks: %ld, ms: %f, s: %lf\n", text.getPtr(), diff, mil, sec);
            }
    };
}

#endif
