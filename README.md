# shared_ptr
Implementation of shared pointer. Incomplete but illustrates key aspects of implementation.


There are a variety of items not included (time constraints)  but the key point is to illustrate the reference counting,
memory cleanup, type erasure and use of deleters.

For example these items are not included.

* bad_alloc isn't caught and thrown in the appropriate ctors, this is deliberate (readability of code). 
* #include <cxxabi.h> is commented out in headers , it was used for debugging to determine the type of U and T
  for base/derived tests
* the move constructors aren't in there but could trivially be added
* weak_ptr and lock() are not included





**BUILD**

% cmake .

% make 

There are zero errors and warnings (the build on linux is set to use -Wall , -Wextra and Wpedantic.
The build disables all optimizations and enables debug code generation.






**TESTING**

Running the tests;

% ./bin/shared_ptr_test

TEST - graham::shared_ptr<base> bp{new base()};

TEST - graham::shared_ptr<base> dp{new derived()};

TEST - graham::shared_ptr<base> bp (new base());

TEST - graham::shared_ptr<base> bp2 (new derived(), delit);

TEST - graham::shared_ptr<base> bp2 (new derived(), [] (base* b){ delete b;});

TEST - graham::shared_ptr<base> bp2 (new derived(), [] (derived* d){ delete d;});

TEST - graham::shared_ptr<base> bp2 (new derived()); (no reset)

TEST - get deleter, expect null

TEST - get_deleter, expect non null

TEST - alias ctor

TEST - multiple bp and assignment

TEST - equality

TEST - nullptr

TEST - equality (empty)

TEST - equality base/derived

TEST - invocation

TEST - operator!

TEST - multiple instances & assignment

TEST - multiple instances & multiple assignment

TEST - multiple base and multiple derived

TEST - swap and get value 0

TEST - get base value

TEST - multiple reset

TEST - unique() true

TEST - unique() false

TEST - operator bool() true

TEST - operator bool() false

TEST - operator<<

use_count: 1,  shared_ptr<T=, U=> , _ptr=0x4dd4d80








**MEMORY LEAK DETECTION**


valgrind can be used to confirm that there are indeed no memory leaks and the shared_ptr implementation cleans up as 
intended.

% valgrind --leak-check=full ./bin/shared_ptr_test

==3879==

==3879== HEAP SUMMARY:

==3879==     in use at exit: 0 bytes in 0 blocks

==3879==   total heap usage: 75 allocs, 75 frees, 76,180 bytes allocated

==3879==

==3879== All heap blocks were freed -- no leaks are possible

==3879==

==3879== For lists of detected and suppressed errors, rerun with: -s

==3879== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)




