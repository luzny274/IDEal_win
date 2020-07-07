#ifndef MAP_BU
#define MAP_BU

#include "Array.cpp"

namespace ulm{

    template<typename Key, typename Value> class Couple{
        public:
            Key x;
            Value y;

            Key&    key()   { return x; };
            Value&  value() { return y; };

            Couple(){}
            Couple(Key k, Value val){ x = k; y = val; };
    };

    template<typename Key, typename Value> class Map : public Array<Couple<Key, Value>>{
        public:
            using Array<Couple<Key, Value>>::Array;

            Map(std::initializer_list<Key> keys, std::initializer_list<Value> values)   { initialize(keys, values); }
            Map(std::initializer_list<Key> keys, Value val)                             { initialize(keys, val);    }
            Map(Key key, std::initializer_list<Value> values)                           { initialize(key , values); }

            void initialize(std::initializer_list<Key> keys, std::initializer_list<Value> values){
                if(keys.size != values.size){
                    printf("\nULM::ERROR::MAP::initialize::std_initializer_lists dont have same size\n");
                    fflush(stdout);
                }

                if(this->array != NULL) this->empty();
                
                int velikost = keys.size();
                this->array = new Couple<Key, Value>[velikost];
                this->velikostReal = velikost;

                const Key* ks  = keys.begin();
                const Value* vals  = values.begin();

                for(int i = 0; i < velikost; i++)
                    this->add(Couple<Key, Value>(ks[i], vals[i]));
            }

            void initialize(std::initializer_list<Key> keys, Value val){
                if(this->array != NULL) this->empty();
                
                int velikost = keys.size();
                this->array =  new Couple<Key, Value>[velikost];
                this->velikostReal = velikost;

                const Key* ks  = keys.begin();

                for(int i = 0; i < velikost; i++)
                    this->add(Couple<Key, Value>(ks[i], val));
            }

            void initialize(Key key, std::initializer_list<Value> vals){
                if(this->array != NULL) this->empty();
                
                int velikost = vals.size();
                this->array =  new Couple<Key, Value>[velikost];
                this->velikostReal = velikost;

                const Value* ks  = vals.begin();

                for(int i = 0; i < velikost; i++)
                    this->add(Couple<Key, Value>(key, vals[i]));
            }

            

            Value getByKey(Key key){
                for(int i = 0; i < this->size; i++)
                    if(this->array[i].key() == key)
                        return this->array[i].value();

                return Value();
            }

            Key getByValue(Value val){
                for(int i = 0; i < this->size; i++)
                    if(this->array[i].value() == val)
                        return this->array[i].key();

                return Key();
            }
    };
}


#endif