#ifndef CONTROL_BLOCK_BASE
#define CONTROL_BLOCK_BASE

#include <atomic>
#include <functional>

// only for debugging, see below
//#include <cxxabi.h>

namespace graham
{

    namespace detail
    {
        struct control_block_base 
        {
            template <typename T, typename Deleter>
                control_block_base(T* t, const Deleter& d) 
                {
                    _cleanup = [=] () 
                    { 
                        const Deleter _d{d};
                        _d(t); 
                    };

                    _deleter = (void*) &d;

                    // only for debugging/operator<<
                    //int status_ignored;
                    //_true_type = abi::__cxa_demangle(typeid(T).name(),NULL,NULL,&status_ignored);
                }

            template <typename T>
                control_block_base(T* t) : _deleter{nullptr}
            {
                _cleanup = [=] () 
                { 
                    delete(t); 
                };


                // only for debugging/operator<<
                //int status_ignored;
                //_true_type = abi::__cxa_demangle(typeid(T).name(),NULL,NULL,&status_ignored);

            }
            virtual ~control_block_base()
            {
            }

            void* get_deleter()
            {
                return _deleter;
            }

            void cleanup()
            {
                _cleanup();
            }

           std::atomic<int> _ref_count;

            private:

            void* _deleter;

            std::function<void()> _cleanup;
        };
    } 
}


#endif
