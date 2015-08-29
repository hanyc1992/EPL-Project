// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility> // for std::move

const int initial_size = 8;

namespace epl{

template <typename T>
class vector {
private:
    T* begin_ca;
    T* end_ca;
    T* begin_st;
    T* end_st;
    uint64_t len_ca;
    uint64_t len_st;
    uint64_t offset;

public:
    vector(void){
        len_ca = initial_size;
        len_st = 0;
        offset = 0;
        begin_ca = (T*) operator new(initial_size * sizeof(T));
        end_ca = begin_ca + initial_size;
        begin_st = begin_ca;
        end_st = begin_ca;
    }

    vector(uint64_t n){
        if (n == 0)
            vector();
        else{
            len_ca = n;
            len_st = n;
            offset = 0;
            begin_ca = (T*) operator new(n * sizeof(T));
            end_ca = begin_ca + n;
            begin_st = begin_ca;
            end_st = end_ca;

            for(T* temptr = begin_ca; temptr != end_ca; ++temptr){
                new(temptr) T{};
            }
        }
    }


    vector(const vector<T>& that){copy(that); }

    vector(vector<T>&& that){
        len_ca = that.len_ca;
        len_st = that.len_st;
        offset = that.offset;
        begin_ca = that.begin_ca;
        begin_st = that.begin_st;
        end_ca = that.end_ca;
        end_st = that.end_st;
        that.begin_ca = that.begin_st = that.end_ca = that.end_st = nullptr;
    }       //move constructor

    vector<T>& operator=(const vector<T>& that){
        if (this != &that){
			destroy();
			copy(that);
		}
		return *this;
    }

    vector<T>& operator=(vector<T>&& that){
        len_ca = that.len_ca;
        len_st = that.len_st;
        offset = that.offset;
        begin_ca = that.begin_ca;
        begin_st = that.begin_st;
        end_ca = that.end_ca;
        end_st = that.end_st;
        that.begin_ca = that.begin_st = that.end_ca = that.end_st = nullptr;

        return *this;
    }       //move assignment operator

    ~vector(void){destroy(); }

    uint64_t size(void) const{return len_st;}

    T& operator[](uint64_t k){
        if (k >= len_st){throw std::out_of_range{"subscript out of range"}; }
        return begin_st[k];
    }

    const T& operator[](uint64_t k) const{
        if (k >= len_st){throw std::out_of_range{"subscript out of range"}; }
        return begin_st[k];
    }


    void push_back(const T& rhs){
        if (len_ca > len_st + offset){
            new(end_st) T{rhs};
            ++len_st;
            ++end_st;
        }
        else{
//          resize_back();
            resize_back_move();
            new(end_st) T{rhs};
            ++len_st;
            ++end_st;
        }
    }

    void push_back(T&& rhs){
        if (len_ca > len_st + offset){
            new(end_st) T{std::move(rhs)};
            ++len_st;
            ++end_st;
        }
        else{
            resize_back_move();
            new(end_st) T{std::move(rhs)};
            ++len_st;
            ++end_st;
        }
    }

    void pop_back(void){
        if (len_st == 0){throw std::out_of_range{"empty vector -- back"}; }
        --end_st;
        end_st -> ~T();
        --len_st;
    }

    void push_front(const T& rhs){
        if (offset > 0){
            --begin_st;
            new(begin_st) T{rhs};
            --offset;
            ++len_st;
        }
        else{
//          resize_front();
            resize_front_move();
            --begin_st;
            new(begin_st) T{rhs};
            --offset;
            ++len_st;
        }
    }

    void push_front(T&& rhs){
        if (offset > 0){
            --begin_st;
            new(begin_st) T{std::move(rhs)};
            --offset;
            ++len_st;
        }
        else{
            resize_front_move();
            --begin_st;
            new(begin_st) T{std::move(rhs)};
            --offset;
            ++len_st;
        }
    }

    void pop_front(void){
        if (len_st == 0){throw std::out_of_range{"empty vector -- front"}; }
        begin_st -> ~T();
        ++begin_st;
        --len_st;
        ++offset;
    }

    void copy(const vector<T>& that){
        len_ca = that.len_ca;
        len_st = that.len_st;
        offset = that.offset;
        begin_ca = (T*) operator new(len_ca * sizeof(T));
        end_ca = begin_ca + len_ca;
        begin_st = begin_ca + offset;
        end_st = begin_st + len_st;

        for(uint32_t i = 0; i < len_st; ++i){
            new(begin_st + i) T{that.begin_st[i]};
        }
    }

    void destroy(void){
        for(T* temptr = begin_st; temptr != end_st; ++temptr){
            temptr -> ~T();
        }
        operator delete(begin_ca);
    }

    void resize_back(void){
        T* temp_begin_ca;
        T* temp_end_ca;
        T* temp_begin_st;
        T* temp_end_st;

        temp_begin_ca = (T*) operator new(2 * len_ca * sizeof(T));
        temp_end_ca = temp_begin_ca + 2 * len_ca;
        temp_begin_st = temp_begin_ca + offset;
        temp_end_st = temp_begin_st + len_st;

        for(uint32_t i = 0; i < len_st; ++i){
            new(temp_begin_st + i) T{begin_st[i]};
        }
        destroy();

        begin_ca = temp_begin_ca;
        end_ca = temp_end_ca;
        begin_st = temp_begin_st;
        end_st = temp_end_st;
        len_ca = 2 * len_ca;
        len_st = len_st;
        offset = offset;
    }

    void resize_front(void){
        T* temp_begin_ca;
        T* temp_end_ca;
        T* temp_begin_st;
        T* temp_end_st;

        temp_begin_ca = (T*) operator new(2 * len_ca * sizeof(T));
        temp_end_ca = temp_begin_ca + 2 * len_ca;
        temp_begin_st = temp_begin_ca + offset + len_ca;
        temp_end_st = temp_begin_st + len_st;

        for(uint32_t i = 0; i < len_st; ++i){
            new(temp_begin_st + i) T{begin_st[i]};
        }
        destroy();

        begin_ca = temp_begin_ca;
        end_ca = temp_end_ca;
        begin_st = temp_begin_st;
        end_st = temp_end_st;
        offset = offset + len_ca;
        len_ca = 2 * len_ca;
        len_st = len_st;
    }


    void resize_back_move(void){
        T* temp_begin_ca;
        T* temp_end_ca;
        T* temp_begin_st;
        T* temp_end_st;

        temp_begin_ca = (T*) operator new(2 * len_ca * sizeof(T));
        temp_end_ca = temp_begin_ca + 2 * len_ca;
        temp_begin_st = temp_begin_ca + offset;
        temp_end_st = temp_begin_st + len_st;

        for(uint32_t i = 0; i < len_st; ++i){
            new(temp_begin_st + i) T{std::move(begin_st[i])};
        }
        begin_ca = begin_st = end_ca = end_st = nullptr;
        destroy();

        begin_ca = temp_begin_ca;
        end_ca = temp_end_ca;
        begin_st = temp_begin_st;
        end_st = temp_end_st;
        len_ca = 2 * len_ca;
        len_st = len_st;
        offset = offset;
    }

    void resize_front_move(void){
        T* temp_begin_ca;
        T* temp_end_ca;
        T* temp_begin_st;
        T* temp_end_st;

        temp_begin_ca = (T*) operator new(2 * len_ca * sizeof(T));
        temp_end_ca = temp_begin_ca + 2 * len_ca;
        temp_begin_st = temp_begin_ca + offset + len_ca;
        temp_end_st = temp_begin_st + len_st;

        for(uint32_t i = 0; i < len_st; ++i){
            new(temp_begin_st + i) T{std::move(begin_st[i])};
        }
        begin_ca = begin_st = end_ca = end_st = nullptr;
        destroy();

        begin_ca = temp_begin_ca;
        end_ca = temp_end_ca;
        begin_st = temp_begin_st;
        end_st = temp_end_st;
        offset = offset + len_ca;
        len_ca = 2 * len_ca;
        len_st = len_st;
    }
};

} //namespace epl

#endif /* _Vector_h */
