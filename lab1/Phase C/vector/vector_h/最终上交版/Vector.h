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
	size_t resize_ver = 0;

public:
	class iterator : public std::iterator<std::random_access_iterator_tag, T> {
	//private:
	public:
		const vector<T>* obj;
		uint64_t version;
		uint64_t resize_ver;
		uint64_t position;
		bool valid;
	public:
		iterator(const vector<T>* obj_vec, size_t version_vec, size_t resize_ver_vec, uint64_t position_vec) {
			obj = obj_vec;
			version = version_vec;
			resize_ver = resize_ver_vec;
			position = position_vec;
			valid = (position >= 0 && position < obj->len_st);
		}
		
		iterator(const iterator& that) : obj(that.obj), version(that.version), resize_ver(that.resize_ver), position(that.position), valid(that.valid) {};

		iterator operator=(const iterator& that) {
			obj = that.obj;
			version = that.version;
			resize_ver = that.resize_ver;
			position = that.position;
			valid = that.valid;
			return *this;
		}

		iterator& operator++() {
			check(this);
			++position;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const iterator operator++(int) {
			iterator old{*this};
			++(*this);
			return old;
		}

		T& operator*() {
			check(this);
			return *(obj->begin_st + position); 
		}

		iterator& operator--() {
			check(this);
			--position;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const iterator operator--(int) {
			iterator old{ *this };
			--(*this);
			return old;
		}

		iterator& operator+(uint64_t k) {
			check(this);
			position += k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		iterator& operator-(uint64_t k) {
			check(this);
			position -= k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		size_t operator-(const iterator& that) {
			check(this);
			check(&that);
			return (this->position - that.position); 
		}

		iterator& operator-=(uint64_t k) {
			check(this);
			position -= k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		iterator& operator+=(uint64_t k) {
			check(this);
			position += k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}


		bool operator==(const iterator& that) const { check(this); return this->position == that.position; }
		bool operator<(const iterator& that) const { check(this); return this->position < that.position; }
		bool operator!=(const iterator& that) const { check(this); return this->position != that.position; }
		bool operator<=(const iterator& that) const { check(this); return this->position <= that.position;}
		bool operator>(const iterator& that) const { check(this); return this->position > that.position;}
		bool operator>=(const iterator& that) const { check(this); return this->position >= that.position; }

		T& operator[](uint64_t k) { return *(obj->begin_st + position + k); }

		/*void check(const iterator* iter) const{
			if (iter->version != iter->obj->version) {
				if (iter->position < 0 || iter->position >= iter->obj->len_st) { throw epl::invalid_iterator{ epl::invalid_iterator::SEVERE }; }
				else if (iter->p < iter->obj->begin_st || iter->p >= iter->obj->end_st) { throw epl::invalid_iterator{ epl::invalid_iterator::MODERATE }; }
				else { throw epl::invalid_iterator{ epl::invalid_iterator::MILD }; }
			}
		}*/

		void check(const iterator* iter) const {
			if ((iter->version != iter->obj->version) || (iter->resize_ver != iter->obj->resize_ver)) {
				if (iter->valid && (iter->position < 0 || iter->position >= iter->obj->len_st)) 
					{ throw epl::invalid_iterator{ epl::invalid_iterator::SEVERE }; }
				else if (iter->valid && (iter->position >= 0 || iter->position < iter->obj->len_st) && (iter->resize_ver != iter->obj->resize_ver)) 
					{ throw epl::invalid_iterator{ epl::invalid_iterator::MODERATE }; }
				else { throw epl::invalid_iterator{ epl::invalid_iterator::MILD }; }
			}
		}
	};

	class const_iterator : public std::iterator<std::random_access_iterator_tag, T> {
	private:
		const vector<T>* obj;
		uint64_t version;
		uint64_t resize_ver;
		uint64_t position;
		bool valid;
	public:
		const_iterator(const vector<T>* obj_vec, size_t version_vec, size_t resize_ver_vec, uint64_t position_vec) {
			obj = obj_vec;
			version = version_vec;
			resize_ver = resize_ver_vec;
			position = position_vec;
			valid = (position >= 0 && position < obj->len_st);
		}

		const_iterator(const const_iterator& that) : obj(that.obj), version(that.version), resize_ver(that.resize_ver), position(that.position), valid(that.valid) {};

		const_iterator(const typename epl::vector<T>::iterator& that) {
			obj = that.obj;
			version = that.version;
			resize_ver = that.resize_ver;
			position = that.position;
			valid = that.valid;
		}

		const_iterator operator=(const const_iterator& that) {
			obj = that.obj;
			version = that.version;
			resize_ver = that.resize_ver;
			position = that.position;
			valid = that.valid;
			return *this;
		}

		const_iterator operator=(const typename epl::vector<T>::iterator& that) {
			obj = that.obj;
			version = that.version;
			resize_ver = that.resize_ver;
			position = that.position;
			valid = that.valid;
			return *this;
		}

		const T& operator*() {
			check(this);
			return *(obj->begin_st + position);
		}
		
		const const_iterator& operator++() {
			check(this);
			++position;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const const_iterator operator++(int) {
			const_iterator old{ *this };
			++(*this);
			return old;
		}

		const const_iterator& operator--() {
			check(this);
			--position;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const const_iterator operator--(int) {
			const_iterator old{ *this };
			--(*this);
			return old;
		}

		const const_iterator& operator+(uint64_t k) {
			check(this);
			position += k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const const_iterator& operator-(uint64_t k) {
			check(this);
			position -= k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		T& operator[](uint64_t k) { return *(obj->begin_st + position + k); }

		size_t operator-(const const_iterator& that){
			check(this);
			check(&that);
			return (this->position - that.position);
		}

		const const_iterator& operator-=(uint64_t k) {
			check(this);
			position -= k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		const const_iterator& operator+=(uint64_t k) {
			check(this);
			position += k;
			valid = (position >= 0 && position < obj->len_st);
			return *this;
		}

		bool operator==(const const_iterator& that) const { check(this); return this->position == that.position; }
		bool operator<(const const_iterator& that) const { check(this); return this->position < that.position; }
		bool operator!=(const const_iterator& that) const { check(this); return this->position != that.position; }
		bool operator<=(const const_iterator& that) const { check(this); return this->position <= that.position; }
		bool operator>(const const_iterator& that) const { check(this); return this->position > that.position; }
		bool operator>=(const const_iterator& that) const { check(this); return this->position >= that.position; }

		void check(const const_iterator* iter) const {
			if ((iter->version != iter->obj->version) || (iter->resize_ver != iter->obj->resize_ver)) {
				if (iter->valid && (iter->position < 0 || iter->position >= iter->obj->len_st))
					{throw epl::invalid_iterator{ epl::invalid_iterator::SEVERE };}
				else if (iter->valid && (iter->position >= 0 || iter->position < iter->obj->len_st) && (iter->resize_ver != iter->obj->resize_ver))
					{throw epl::invalid_iterator{ epl::invalid_iterator::MODERATE };}
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
	vector<T>& vector_my(iter b, iter e, std::random_access_iterator_tag) {
		size_t dist = e - b;
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
		return *this;
	}

	template < typename iter >
	vector<T>& vector_my(iter b, iter e, std::input_iterator_tag) {
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

		return *this;
	}

	template < typename iter >
	vector(iter b, iter e) {
		typename std::iterator_traits<iter>::iterator_category x{};
		*this = vector_my<iter>(b, e, x);
	}

	const_iterator begin(void) const{ return const_iterator( this, version, resize_ver, 0 ); }
	const_iterator end(void) const{ return const_iterator(this, version, resize_ver, len_st);}
	iterator begin(void) { return iterator(this, version, resize_ver, 0);}
	iterator end(void) { return iterator(this, version, resize_ver, len_st);}

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
			++resize_ver;
		}
		++version;
		return *this;
    }

    vector<T>& operator=(vector<T>&& that){
		++version;
		++resize_ver;
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
			++resize_ver;
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
			++resize_ver;
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
			++resize_ver;
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
			++resize_ver;
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
