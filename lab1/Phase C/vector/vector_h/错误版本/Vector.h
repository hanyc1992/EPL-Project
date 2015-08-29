// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility> // for std::move
#include <iterator>
#include <algorithm>

const int initial_size = 8;




//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl {

class invalid_iterator {
public:
	enum SeverityLevel { SEVERE, MODERATE, MILD, WARNING };
	SeverityLevel level;

	invalid_iterator(SeverityLevel level = SEVERE) { this->level = level; }
	virtual const char* what() const {
		switch (level) {
		case WARNING:   return "Warning"; // not used in Spring 2015
		case MILD:      return "Mild";
		case MODERATE:  return "Moderate";
		case SEVERE:    return "Severe";
		default:        return "ERROR"; // should not be used
		}
	}
};

template <typename T>
class vector {
private:
    T* begin_ca;
    T* end_ca;
    T* begin_st;
    T* end_st;
	size_t len_ca;
	size_t len_st;
	size_t offset;
	size_t version = 0;

public:
	class iterator : public std::iterator<std::random_access_iterator_tag, T> {
	//private:
	public:
		T* p;
		const vector<T>* obj;
		uint64_t version;
		uint64_t position;
	public:
		iterator(T* p_vec, const vector<T>* obj_vec, size_t version_vec) { p = p_vec; obj = obj_vec; version = version_vec; position = std::distance(obj->begin_st, p_vec); };
		iterator(const iterator& that) : p(that.p), obj(that.obj), version(that.version), position(that.position) {};
		iterator operator=(const iterator& that) { p = that.p; obj = that.obj;  version = that.version; position = that.position; return *this; };

		iterator& operator++() {
			check(this);
			(*this).p += 1;
			++position;
			return *this;
		}

		const iterator operator++(int) {
			iterator old{*this};
			++(*this);
			return old;
		}

		value_type& operator*() {
			check(this);
			return *p; 
		}

		iterator& operator--() {
			check(this);
			(*this).p -= 1;
			--position;
			return *this;
		}

		const iterator operator--(int) {
			iterator old{ *this };
			--(*this);
			return old;
		}

		iterator& operator+(uint64_t k) {
			check(this);
			(*this).p += k;
			position += k;
			return *this;
		}

		iterator& operator-(uint64_t k) {
			check(this);
			(*this).p -= k;
			position -= k;
			return *this;
		}

		difference_type operator-(const iterator& that) {
			check(this);
			check(&that);
			return (this->p - that.p); 
		}



		iterator& operator-=(uint64_t k) {
			check(this);
			(*this).p -= k;
			position -= k;
			return *this;
		}

		iterator& operator+=(uint64_t k) {
			check(this);
			(*this).p += k;
			position += k;
			return *this;
		}


		bool operator==(const iterator& that) const { check(this); return this->p == that.p; }
		bool operator<(const iterator& that) const { check(this); return this->p < that.p; }
		bool operator!=(const iterator& that) const { check(this); return this->p != that.p; }
		bool operator<=(const iterator& that) const { check(this); return this->p <= that.p;}
		bool operator>(const iterator& that) const { check(this); return this->p > that.p;}
		bool operator>=(const iterator& that) const { check(this); return this->p >= that.p; }

		value_type& operator[](uint64_t k) { return *(p + k); }

		void check(const iterator* iter) const{
			if (iter->version != iter->obj->version) {
				if (iter->position < 0 || iter->position >= iter->obj->len_st) { throw epl::invalid_iterator{ epl::invalid_iterator::SEVERE }; }
				else if (iter->p < iter->obj->begin_st || iter->p >= iter->obj->end_st) { throw epl::invalid_iterator{ epl::invalid_iterator::MODERATE }; }
				else { throw epl::invalid_iterator{ epl::invalid_iterator::MILD }; }
			}
		}
	};

	class const_iterator : public std::iterator<std::random_access_iterator_tag, T> {
	private:
		T* p;
		const vector<T>* obj;
		uint64_t version;
		uint64_t position;
	public:
		const_iterator(T* p_vec, const vector<T>* obj_vec, size_t version_vec) { p = p_vec; obj = obj_vec; version = version_vec; position = std::distance(obj->begin_st, p_vec); };
		const_iterator(const const_iterator& that) : p(that.p), obj(that.obj), version(that.version), position(that.position) {};
		const_iterator(const typename epl::vector<T>::iterator& that) { p = that.p; obj = that.obj;  version = that.version; position = that.position; };
		const_iterator operator=(const const_iterator& that) { p = that.p; obj = that.obj;  version = that.version; position = that.position; return *this; };
		const_iterator operator=(const typename epl::vector<T>::iterator& that) { p = that.p; obj = that.obj;  version = that.version; position = that.position; return *this; };

		const value_type& operator*() { check(this); return *p; }
		
		const const_iterator& operator++() {
			check(this);
			(*this).p += 1;
			++position;
			return *this;
		}

		const const_iterator operator++(int) {
			const_iterator old{ *this };
			++(*this);
			return old;
		}

		const const_iterator& operator--() {
			check(this);
			(*this).p -= 1;
			--position;
			return *this;
		}

		const const_iterator operator--(int) {
			const_iterator old{ *this };
			--(*this);
			return old;
		}

		const const_iterator& operator+(uint64_t k) {
			check(this);
			(*this).p += k;
			position += k;
			return *this;
		}

		const const_iterator& operator-(uint64_t k) {
			check(this);
			(*this).p -= k;
			position -= k;
			return *this;
		}

		value_type& operator[](uint64_t k) { return *(p + k); }

		const const_iterator& operator-=(uint64_t k) {
			check(this);
			(*this).p -= k;
			position -= k;
			return *this;
		}

		const const_iterator& operator+=(uint64_t k) {
			check(this);
			(*this).p += k;
			position += k;
			return *this;
		}






		difference_type operator-(const const_iterator& that) { check(this); check(&that); return this->p - that.p; }

		bool operator==(const const_iterator& that) const { check(this); return this->p == that.p; }
		bool operator<(const const_iterator& that) const { check(this); return this->p < that.p; }
		bool operator!=(const const_iterator& that) const { check(this); return this->p != that.p; }
		bool operator<=(const const_iterator& that) const { check(this); return this->p <= that.p; }
		bool operator>(const const_iterator& that) const { check(this); return this->p > that.p; }
		bool operator>=(const const_iterator& that) const { check(this); return this->p >= that.p; }

		void check(const const_iterator* iter) const {
			if (iter->version != iter->obj->version) {
				if (iter->position < 0 || iter->position >= iter->obj->len_st) { throw epl::invalid_iterator{ epl::invalid_iterator::SEVERE }; }
				else if (iter->p < iter->obj->begin_st || iter->p >= iter->obj->end_st) { throw epl::invalid_iterator{ epl::invalid_iterator::MODERATE }; }
				else { throw epl::invalid_iterator{ epl::invalid_iterator::MILD }; }
			}
		}
	};

	vector(std::initializer_list<T> args) {
		len_ca = args.size();
		len_st = len_ca;
		offset = 0;
		begin_ca = (T*) operator new(len_ca * sizeof(T));
		end_ca = begin_ca + len_ca;
		begin_st = begin_ca;
		end_st = end_ca;

		auto ptr = args.begin();
		auto ptr_vec = begin_st;

		for (; ptr != args.end(); ++ptr, ++ptr_vec)
			new(ptr_vec) T{ *ptr };
	}

	template < typename iter >
	vector(iter b, iter e, std::random_access_iterator_tag) {
		typename std::iterator_traits<iter>::difference_type dist = e - b;
		if (dist <= 0) {
			len_ca = initial_size;
			len_st = 0;
			offset = 0;
			begin_ca = (T*) operator new(initial_size * sizeof(T));
			end_ca = begin_ca + initial_size;
			begin_st = begin_ca;
			end_st = begin_ca;
			//vector();
		}
		else {
			len_ca = dist;
			len_st = dist;
			offset = 0;
			begin_ca = (T*) operator new(dist * sizeof(T));
			end_ca = begin_ca + dist;
			begin_st = begin_ca;
			end_st = end_ca;

			iter source = b;
			for (T* temptr = begin_ca; temptr != end_ca; ++temptr) {
				new(temptr) T{*source};
				++source;
			}
		}
	}

	template < typename iter >
	vector(iter b, iter e, std::input_iterator_tag) {
		len_ca = initial_size;
		len_st = 0;
		offset = 0;
		begin_ca = (T*) operator new(initial_size * sizeof(T));
		end_ca = begin_ca + initial_size;
		begin_st = begin_ca;
		end_st = begin_ca;
		//vector<T>();
		for (auto source = b; source != e; ++source)
			push_back(*source);
	}

	template < typename iter >
	vector(iter b, iter e) {
		typename std::iterator_traits<iter>::iterator_category x{};
		vector<T>(b, e, x);
	}

	const_iterator begin(void) const{ return const_iterator( begin_st, this, version ); }
	const_iterator end(void) const{ return const_iterator( end_st, this, version ); }
	iterator begin(void) { return iterator(begin_st, this, version); }
	iterator end(void) { return iterator(end_st, this, version); }

    vector(void){
        len_ca = initial_size;
        len_st = 0;
        offset = 0;
        begin_ca = (T*) operator new(initial_size * sizeof(T));
        end_ca = begin_ca + initial_size;
        begin_st = begin_ca;
        end_st = begin_ca;
    }

    vector(size_t n){
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
		if (this != &that) {
			destroy();
			copy(that);
		}
		++version;
		return *this;
    }

    vector<T>& operator=(vector<T>&& that){
		++version;
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

    size_t size(void) const{return len_st;}

    T& operator[](size_t k){
        if (k >= len_st){throw std::out_of_range{"subscript out of range"}; }
        return begin_st[k];
    }

    const T& operator[](size_t k) const{
        if (k >= len_st){throw std::out_of_range{"subscript out of range"}; }
        return begin_st[k];
    }

    void push_back(const T& rhs){
		++version;
        if (len_ca > len_st + offset){
            new(end_st) T{rhs};
            ++len_st;
            ++end_st;
        }
        else{
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
            new(temp_end_st) T{rhs};

            begin_ca = begin_st = end_ca = end_st = nullptr;
            destroy();

            begin_ca = temp_begin_ca;
            end_ca = temp_end_ca;
            begin_st = temp_begin_st;
            end_st = temp_end_st;
            len_ca = 2 * len_ca;
            len_st = len_st;
            offset = offset;

            ++len_st;
            ++end_st;
        }
    }

    void push_back(T&& rhs){
		++version;
        if (len_ca > len_st + offset){
            new(end_st) T{std::move(rhs)};
            ++len_st;
            ++end_st;
        }
        else{
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
            new(temp_end_st) T{std::move(rhs)};

            begin_ca = begin_st = end_ca = end_st = nullptr;
            destroy();

            begin_ca = temp_begin_ca;
            end_ca = temp_end_ca;
            begin_st = temp_begin_st;
            end_st = temp_end_st;
            len_ca = 2 * len_ca;
            len_st = len_st;
            offset = offset;

            ++len_st;
            ++end_st;
        }
    }

    void pop_back(void){
		++version;
        if (len_st == 0){throw std::out_of_range{"empty vector -- back"}; }
        --end_st;
        end_st -> ~T();
        --len_st;
    }

    void push_front(const T& rhs){
		++version;
        if (offset > 0){
            --begin_st;
            new(begin_st) T{rhs};
            --offset;
            ++len_st;
        }
        else{
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
            --temp_begin_st;
            new(temp_begin_st) T{rhs};

            begin_ca = begin_st = end_ca = end_st = nullptr;
            destroy();

            begin_ca = temp_begin_ca;
            end_ca = temp_end_ca;
            begin_st = temp_begin_st;
            end_st = temp_end_st;
            offset = offset + len_ca;
            len_ca = 2 * len_ca;
            len_st = len_st;

            --offset;
            ++len_st;
        }
    }

    void push_front(T&& rhs){
		++version;
        if (offset > 0){
            --begin_st;
            new(begin_st) T{std::move(rhs)};
            --offset;
            ++len_st;
        }
        else{
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
            --temp_begin_st;
            new(temp_begin_st) T{std::move(rhs)};

            begin_ca = begin_st = end_ca = end_st = nullptr;
            destroy();

            begin_ca = temp_begin_ca;
            end_ca = temp_end_ca;
            begin_st = temp_begin_st;
            end_st = temp_end_st;
            offset = offset + len_ca;
            len_ca = 2 * len_ca;
            len_st = len_st;

            --offset;
            ++len_st;
        }
    }

    void pop_front(void){
		++version;
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

};

} //namespace epl

#endif /* _Vector_h */
