#ifndef BYTEARRAY_BU
#define BYTEARRAY_BU

#include "Array.cpp"

namespace ulm{
    class ByteArray : public Array<unsigned char>{
        public:
            using Array::Array;

            void append(unsigned char prvek){
                if(this->size >= this->velikostReal){
                    this->velikostReal *= 2;
                    this->resize();
                }
                this->array[size] = prvek;
                this->size++;
            }

            void add(unsigned char * bytes, int arg_size){
                for(int i = 0; i < arg_size; i++)
                    append(bytes[i]);
            }

            template<typename typ> void add(typ value){ 
                add(reinterpret_cast<unsigned char *>(&value), sizeof(typ));
            }


            template<typename typ> void initialize(std::initializer_list<typ> list){
                if(this->array != NULL) this->empty();
                
                int velikost = list.size();
                this->array = new unsigned char[velikost * sizeof(typ)];
                this->velikostReal = velikost;

                const typ* p  = list.begin();

                for(int i = 0; i < velikost; i++)
                    add(p[i]);
            }

            template<typename typ> void initialize(Array<typ>& pole){
                for(int i = 0; i < pole.size; i++)
                    add(reinterpret_cast<unsigned char *>(&pole[i]), sizeof(typ));
            }


            template<typename typ> static ByteArray parse(Array<typ>& pole){
                ByteArray bytes;
                bytes.initialize(pole);
                return(bytes);
            }

            template<typename typ> static ByteArray parse(std::initializer_list<typ> list){
                ByteArray bytes;
                bytes.initialize(list);
                return(bytes);
            }
};
}

#endif