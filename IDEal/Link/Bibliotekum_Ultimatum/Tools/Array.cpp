#ifndef ARRAY_BU
#define ARRAY_BU

#include "../Base/base.hpp"

namespace ulm{

    template<typename typ> class Array{
        protected:        

            bool check(int index){
                if(index < 0){
                    return(0);
                }
                
                if(index >= size && warning){
                    printf("\nULM::ARRAY::WARNING::INDEX>=SIZE::index=%d size=%d\n", index, size);
                    fflush(stdout);
                }

                if(index >= velikostReal){
                    velikostReal = 2 * index;
                    resize();

                    for(int i = size; i < index + 1; i++){
                        array[i] = typ();
                    }
                }

                if(index >= size){
                    size = index + 1;
                }

                return(1);
            }

            void resize(){
                typ * newarr = new typ[velikostReal];

                for(int i = 0; i < size; i++){
                    newarr[i] = array[i];
                }

                delete[] array;

                array = newarr;
            }

        public:
            int velikostReal = 1;
            typ * array = NULL;
            int size = 0;
            bool warning = true;

            Array(){
                array = new typ[1];
                size = 0;
                velikostReal = 1;
            }
            Array(int velikost){
                initialize(velikost);
            }
            Array(int velikost, bool pridat){
                initialize(velikost, pridat);
            }
            Array(std::initializer_list<typ> list) {
                loadFromList(list);
            }
            Array(const Array<typ>& other) {
                copy(other);
            }

            ~Array(){
                delete[] array;
            }  

            bool contains(typ item){
                for(int i = 0; i < size; i++){
                    if(array[i] == item) return(true);
                }
                return false;
            }

            int indexOf(typ item){
                for(int i = 0; i < size; i++){
                    if(array[i] == item) return(i);
                }
                return(-1);
            }  

            void initialize(int velikost, bool pridat){
                initialize(velikost);
                if(pridat) size = velikost;
            }

            void initialize(int velikost){
                if(array != NULL) delete[] array;

                array = new typ[velikost];
                velikostReal = velikost;
                size = 0;
            }

            void loadFromList(std::initializer_list<typ> list){
                if(array != NULL) empty();
                
                int velikost = list.size();
                array =  new typ[velikost];
                velikostReal = velikost;

                const typ* it  = list.begin();
                const typ* end = list.end();

                for (; it != end; ++it) 
                    add(*it);
            }

            bool isEmpty(){
                return(size == 0);
            }

            void empty(int n){
                if(array != NULL) delete[] array;
                if(n < 1) n = 1;
                array = new typ[n];
                size = 0;
                velikostReal = n;
            }

            void empty(){
                if(array != NULL) delete[] array;
                array = new typ[1];
                size = 0;
                velikostReal = 1;
            }

            void expand(int n){
                velikostReal += n;
                resize();
            }

            void copy(const Array<typ>& other){
                if(array != NULL) delete[] array;

                array = new typ[other.velikostReal];
                size = other.size;
                warning = other.warning;
                velikostReal = other.velikostReal;

                for(int i = 0; i < other.size; i++){
                    array[i] = other.array[i];
                }
            }

            void disableWarnings(){
                warning = false;
            } 

            void add(Array<typ>& pole){
                for(typ& prvek : pole)
                    add(prvek);
            }
            void add(Array<typ> pole){
                for(typ& prvek : pole)
                    add(prvek);
            }

            void add(typ prvek){
                if(size >= velikostReal){
                    velikostReal *= 2;
                    resize();
                }
                array[size] = prvek;
                size++;
            }
            void add(std::initializer_list<typ> list){
                const typ* it  = list.begin();
                const typ* end = list.end();

                for (; it != end; ++it) 
                    add(*it);
            }

            typ * getPtr(){
                return(array);
            }

            typ * getPtr(int index){
                check(index);
                return(&array[index]);
            }

            typ& get(int index){
                check(index);
                return(array[index]);
            }

            typ& last(){
                return(get(size - 1));
            }

            typ* begin() { return &array[0]; }
            const typ* begin() const { return &array[0]; }
            typ* end() { return &array[size]; }
            const typ* end() const { return &array[size]; }

            typ & operator [](int index) {
                check(index);
                return array[index];
            }

            Array<typ> operator+ (const Array<typ>& other){
                Array<typ> result(size + other.size);
                result.size = size + other.size;
                
                for(int i = 0; i < size; i++){
                    result[i] = array[i];
                }

                for(int i = 0; i < other.size; i++){
                    result[size + i] = other.array[i];
                }

                return(result);
            }

            Array<typ>& operator+= (std::initializer_list<typ> list){
                add(list);
                return(*this);
            }
            Array<typ>& operator+= (const typ& other){
                add(other);
                return(*this);
            }

            Array<typ>& operator= (const Array<typ>& other){
                copy(other);
                return(*this);
            }

            Array<typ>& operator= (std::initializer_list<typ> list){
                loadFromList(list);
                return(*this);
            }
    };

}

#endif