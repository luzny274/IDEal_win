#ifndef TYPES_BU
#define TYPES_BU

#include "../Base/base.hpp"

namespace ulm{
    enum Type{
        NO_TYPE = -1,
        FLOAT   =  0,
        INT     =  1,
        UINT    =  2,
        CHAR    =  3,
        UCHAR   =  4,
        BYTE    =  4
    };

    class Types{
        public:
            static int sizeOf(Type typ){
                switch(typ){
                    case NO_TYPE:
                        return -1;
                        break;
                        
                    case FLOAT:
                        return sizeof(float);
                        break;

                    case INT:
                        return sizeof(int);
                        break;

                    case UINT:
                        return sizeof(unsigned int);
                        break;

                    case CHAR:
                        return sizeof(char);
                        break;

                    case BYTE:
                        return sizeof(unsigned char);
                        break;
                }

                return -1;
            }

            static unsigned int   toGL    (Type         typ);
            static Type           fromGL  (unsigned int typ);
    };
}

#endif