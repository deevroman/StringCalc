#ifndef VECTOR_H
#define VECTOR_H

namespace my_stl {
    template <class T>
    class vector{
        typedef T* iterator;
        typedef const T* const_iterator;
    private:
        T *data;
        size_t CAPACITY = 0;
        size_t SIZE = 0;
        const double MAGIC = 1.61;
    public:
        vector(){
            data = new T[0];
        }
        vector(const vector& a) {
            CAPACITY = SIZE = a.size();
            data = new T[SIZE];
            for(size_t i = 0; i < a.size(); i++){
                data[i] = a[i];
            }
        }
        vector(size_t n, const T& elem){
            SIZE = CAPACITY = n;
            data = new T[SIZE];
            for(size_t i = 0; i < SIZE; i++){
                data[i] = elem;
            }
        }
        ~vector(){
            delete[] data;
        }
        void push_back(T elem){
            if(SIZE + 1 >= CAPACITY){
                CAPACITY = (size_t)(MAGIC * CAPACITY) + 1;
                T *tmp = new T[CAPACITY];
                for (size_t i = 0; i < SIZE; i++){
                    tmp[i] = data[i];
                }
                delete[] data;
                data = tmp;
            }
            data[SIZE] = elem;
            ++SIZE;
        }
        size_t size() const {
            return SIZE;
        }
        bool empty() const{
            return SIZE == 0;
        }
        T operator[](size_t ind) const {
            return data[ind];
        }
        T& operator[](size_t ind) {
            return data[ind];
        }
        void reverse(){
            for(size_t i = 0; i < SIZE / 2; i++){
                std::swap(data[i], data[SIZE - i - 1]);
            }
        }
        iterator begin() {
            return &data[0];
        }
        const_iterator begin() const {
            return &data[0];
        }
        iterator end() {
            return &data[SIZE];
        }
        const_iterator end() const {
            return &data[SIZE];
        }

    };
}
#endif // VECTOR_H
