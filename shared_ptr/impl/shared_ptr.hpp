#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <iostream>

#include "../detail/control_block.hpp"

// for debugging only
//#include <cxxabi.h>


namespace graham
{
    template <typename T> 
        class shared_ptr
        {
            using element_type = T;
            std::string _true_type;

            // for the case where we are handling base and derived
            // shared pointers
            template <typename>
                friend class shared_ptr;

            void inc()
            {
                ++_cb->_ref_count;
            }

            int dec()
            {
                if (!--_cb->_ref_count)
                {
                    _cb->cleanup();
                    return 1;
                }
                return 0;
            }

            public:

            ~shared_ptr()
            {
                if (use_count())
                {
                    dec();
                }
                if (_cb && !use_count())
                {
                    delete _cb;

                }
            } 

            constexpr shared_ptr() noexcept : _ptr(nullptr), _cb(nullptr) 
            {
            } 

            constexpr shared_ptr(std::nullptr_t ptr) noexcept : _ptr(ptr),  _cb(ptr)
            {
            }

            explicit shared_ptr(T* ptr) : _ptr(ptr), _cb (new detail::control_block<T>(ptr))
            {
                _cb->_ref_count = 1;
            }

            shared_ptr(const shared_ptr<T>& other)
            {
                if (( _cb = other._cb))
                {
                    _ptr = other._ptr;
                    inc();
                }
            }

            template <typename Deleter>
                shared_ptr(T* ptr, Deleter d) : _ptr(ptr), _cb(new detail::control_block<T>(ptr, d))
            {
                _cb->_ref_count = 1;
            }

            template <typename U>
                explicit shared_ptr(U* ptr) : _ptr(ptr),
                _cb (new detail::control_block<U>(ptr, std::default_delete<U>()))
            {
                _cb->_ref_count = 1;
            }

            template <typename U, typename Deleter>
                shared_ptr(U* ptr, Deleter d) :  _ptr(ptr),
                _cb (new detail::control_block<U>(ptr, d))
            {
                _cb->_ref_count = 1;
            }


            template <typename U>
                shared_ptr(shared_ptr<U>& u, element_type* e) : _ptr(e), _cb(u._cb)
            {  
                if (_cb)
                {
                    inc();
                }
            }

            template <typename U>
                shared_ptr(const shared_ptr<U>& u) : _ptr(u._ptr), _cb(u._cb)
            {
                if (_cb)
                {
                    inc();
                }
            }

            template <typename U, typename D>
                shared_ptr(std::unique_ptr<U,D>&& up) : _cb(new detail::control_block<U>(up.get(), up.get_deleter()))
            {
            }


            template <typename U>
                bool operator==(const shared_ptr<U>& other) const noexcept
                {
                    return _ptr == other._ptr;
                }

            template <typename U>
                bool operator!=(const shared_ptr<U>& other) const noexcept
                {
                    return !(*this == other);
                }

            explicit operator bool() const noexcept
            {
                return _ptr != nullptr;
            }

            T* operator->() const noexcept
            {
                return _ptr;
            }


            T& operator*() const noexcept
            {
                return *_ptr;
            }

            void swap(shared_ptr& other)
            {
                std::swap(_cb, other._cb);
                std::swap(_ptr, other._ptr);
            }

            shared_ptr& operator=(const shared_ptr& other) 
            {
                shared_ptr tmp(other);
                swap(tmp);
                return *this;
            }

            // Assignment, copy swap idiom
            template <typename U>
                shared_ptr<T>& operator=(const shared_ptr<U>& other)
                {
                    shared_ptr temp(other);
                    this->swap(temp);
                    return *this;
                }

            template <typename U>
                shared_ptr<T>& operator=(shared_ptr<U>&& other)
                {
                    shared_ptr temp(std::move(other));
                    this->swap(temp);
                    return *this;
                }


            T* get() const noexcept
            {
                return _ptr;
            }

            void reset() noexcept
            {
                if (_cb && _cb->_ref_count > 0)
                {
                    dec();
                }
            }

            long use_count() const noexcept
            {
                return (_cb ? _cb->_ref_count.load() : 0);
            }

            void* get_deleter() const
            {
                return _cb->get_deleter();
            }

            // until c++ 20
            bool unique()
            {
                return _cb->_ref_count == 1;
            }
                

            template <typename U>
                friend std::ostream& operator<<(std::ostream& os, shared_ptr<U>& sp);

            private:

            T* _ptr = nullptr;
            detail::control_block_base *_cb = nullptr;

        };

    template <typename Deleter, typename U>
        Deleter* get_deleter(const shared_ptr<U>& up)
        {
            return (Deleter*) up.get_deleter();
        }

    // This is merely for debugging, useful though.
    template <typename U>
        std::ostream& operator<<(std::ostream& os, shared_ptr<U>& sp)
        {

            os << "use_count: ";
            if (sp._cb && sp.use_count())
            {
                //int status_ignored;
                os << sp.use_count() 
                    << ",  shared_ptr<T="
                    // << abi::__cxa_demangle(typeid(*(static_cast<control_block<U>*>((sp._cb))->_ptr)).name(),NULL,NULL,&status_ignored)
                    << ", U=" 
                    << sp._true_type
                    << "> , _ptr="
                    << sp._ptr;
            }
            else
            {
                os << "nullptr/empty";
            }
            return os;
        };

}

#endif
