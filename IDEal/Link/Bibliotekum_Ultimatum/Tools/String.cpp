#ifndef STRING_BU
#define STRING_BU

#include "Array.cpp"

namespace ulm{
    class String{
        public:
            unsigned int cursor = 0;
            Array<char> array;

            String(){ array.add('\0'); }
            String(double num){ copy(num); }
            String(float num){ copy(num); }
            String(int num){ copy(num); }
            String(const char * text){ copy(text); }
            String(const String& other){ copy(other); }
            String(std::initializer_list<char> list){ array = Array<char>(list); }

            char * getPtr(){ return(array.array); }

            void copy(double number){
                array.empty(snprintf(NULL, 0, "%lf", number) + 1);
                sprintf(array.getPtr(), "%lf", number);
            }

            void copy(float number){
                array.empty(snprintf(NULL, 0, "%f", number) + 1);
                sprintf(array.getPtr(), "%f", number);
            }

            void copy(int number){
                array.empty(snprintf(NULL, 0, "%d", number) + 1);
                sprintf(array.getPtr(), "%d", number);
            }

            void copy(const char * text){
                array.empty();
                for(int i = 0; text[i] != '\0'; i++){
                    array.add(text[i]);
                }
                array.add('\0');
            }

            void print(){
                for(int i = 0; array[i] != '\0'; i++){
                    printf("%c", array[i]);
                }
            }

            void copy(const String& other){
                array = other.array;
                cursor = other.cursor;
            }

            void load(const char * path){
                FILE * f = fopen(path, "r");

                char c;
                while(fscanf(f, "%c", &c) != EOF){
                    array.add(c);
                }

                fclose(f);
            }


            

            void append(char c){
                array[array.size - 1] = c;
                array.add('\0');
            }
            void append(double number){
                char * str = (char*)malloc(snprintf(NULL, 0, "%lf", number) + 1);
                sprintf(str, "%lf", number);
                append(str);
                free(str);
            }
            void append(float number){
                char * str = (char*)malloc(snprintf(NULL, 0, "%f", number) + 1);
                sprintf(str, "%f", number);
                append(str);
                free(str);
            }
            void append(int number){
                char * str = (char*)malloc(snprintf(NULL, 0, "%d", number) + 1);
                sprintf(str, "%d", number);
                append(str);
                free(str);
            }
            void append(const char * str){
                array[array.size - 1] = str[0];

                for(int i = 1; str[i] != '\0'; i++){
                    array.add(str[i]);
                }
                array.add('\0');
            }

            void append(const String& other){
                array[array.size - 1] = other.array.array[0];

                for(int i = 1; other.array.array[i] != '\0'; i++){
                    array.add(other.array.array[i]);
                }
                array.add('\0');
            }

            bool contains(char znak){
                for(char z : array){
                    if(z == znak) return(true);
                }

                return(false);
            }

            String substring(int start){ return substring(start, array.size); }

            String substring(int start, int end){
                String result;

                for(int i = start; i < end; i++){
                    result.append(array[i]);
                }

                result.append('\0');

                return result;
            }

            int indexOf(char znak){
                for(char z : array){
                    if(z == znak) return(true);
                }

                return(false);
            }

            String operator+(double number){
                String result = *this;
                result.append(number);
                return result;
            }
            String operator+(float number){
                String result = *this;
                result.append(number);
                return result;
            }
            String operator+(int number){
                String result = *this;
                result.append(number);
                return result;
            }
            String operator+(char c){
                String result = *this;
                result.append(c);
                return result;
            }
            String operator+(const char * text){
                String result = *this;
                result.append(text);
                return result;
            }
            String operator+(const String& other){
                String result = *this;
                result.append(other);
                return result;
            }

            bool operator==(const String& other)
            {
                if(array.size != other.array.size) return(false);

                for(int i = 0; i < array.size; i++){
                    if(array.array[i] != other.array.array[i]) return(false);
                }

                return(true);
            }

            bool operator==(const char * other)
            {
                for(int i = 0; i < array.size; i++){
                    if(other[i] != array[i]) return(false);
                }

                return(true);
            }

            String& operator+=(double number){
                append(number);
                return(*this);
            }
            String& operator+=(float number){
                append(number);
                return(*this);
            }
            String& operator+=(int number){
                append(number);
                return(*this);
            }
            String& operator+=(char c){
                append(c);
                return(*this);
            }
            String& operator+=(const char * text){
                append(text);
                return(*this);
            }
            String& operator+=(const String& other){
                append(other);
                return(*this);
            }
            
            String& operator=(double number){
                copy(number);
                return *this;
            }
            String& operator=(float number){
                copy(number);
                return *this;
            }
            String& operator=(int number){
                copy(number);
                return *this;
            }
            String& operator=(char c){
                copy(c);
                return *this;
            }
            String& operator=(const char * text){
                copy(text);
                return *this;
            }
            String& operator=(const String& other){
                copy(other);
                return *this;
            }

            char& operator [](int index) { return array[index]; }
            char* begin() { return &array.array[0]; }
            const char* begin() const { return &array.array[0]; }
            char* end() { return &array.array[array.size]; }
            const char* end() const { return &array.array[array.size]; }

    };
}

#endif