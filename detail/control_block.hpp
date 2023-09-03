#ifndef CONTROL_BLOCK_HPP
#define CONTROL_BLOCK_HPP

#include "control_block_base.hpp"

// only for debugging, see below
//#include <cxxabi.h>


namespace graham
{

    namespace detail
    {
        template <typename T>
            struct control_block   : public control_block_base
        {

            control_block(nullptr_t ) : _ptr{nullptr}
            {
            }


            control_block(T* ptr) : control_block_base(ptr) ,
            _ptr{ptr} 
            {
            };

            template <typename U, typename Deleter>
                control_block(U* ptr, const Deleter& d) : control_block_base(ptr, std::move(d)) ,
                _ptr{ptr} 
            {
            }

            ~control_block()
            {
            }

            T* get()
            {
                return _ptr;
            }

            T* _ptr;

        };
    } 
} 

#endif
