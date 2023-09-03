#include <iostream>
#include <memory>
#include <ostream>
#include "shared_ptr.hpp"
#include <cassert>

int base_counter = 0;
int derived_counter = 0;

struct base
{

    // non virtual, deliberately, shared_ptr's deleter will delete in terms of the correct type, T or U
    ~base()
    {
        --base_counter;
    }

    base()
    {
        ++base_counter;
    }

    base(int i) : _i(i)
    {
        ++base_counter;
    }

    void foo()
    {
        std::cout  << "base::foo() " << std::endl;
    }

    int get()
    {
        return _i;
    }

    int _i;
};


struct derived : public base
{
    derived()
    {
        ++derived_counter;
    };

    derived(int i) : base(i)
    {
        ++derived_counter;
    };

    ~derived()
    {
        --derived_counter;
    };

    void foo()
    {
        std::cout  << "derived::foo() " << std::endl;
    }
};

struct unrelated
{
};

void delit(derived* d)
{
    delete d;
}

void dbase(base* b)
{
    delete b;
}

int main()
{
    std::cout << std::endl << std::endl;



    int i = 0;
    assert(derived_counter==0 && base_counter == 0);
    std::cout << std::endl << std::endl;
    {
        std::cout << "TEST - graham::shared_ptr<base> bp{new base()};" << std::endl;
        graham::shared_ptr<base> bp{new base()};
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - graham::shared_ptr<base> dp{new derived()}; " << std::endl;
        graham::shared_ptr<base> dp{new derived()};
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - graham::shared_ptr<base> bp (new base()); " << std::endl;
        graham::shared_ptr<base> bp (new base());
        bp.reset();
        assert(derived_counter==0 && base_counter == 0);
    }

    {
        graham::shared_ptr<base> bp;
    }
    assert(derived_counter==0 && base_counter == 0);


    {
        std::cout << "TEST - graham::shared_ptr<base> bp2 (new derived(), delit); " << std::endl;
        graham::shared_ptr<base> bp2 (new derived(), delit);
        assert(derived_counter==1 && base_counter == 1);
        bp2.reset();
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        // ~base() is not virtual, hence derived will not be destructed, deliberate
        std::cout << "TEST - graham::shared_ptr<base> bp2 (new derived(), [] (base* b){ delete b;}); " << std::endl;
        graham::shared_ptr<base> bp2 (new derived(), [] (base* b){ delete b;});
        bp2.reset();
    }
    assert(derived_counter==1 && base_counter == 0);
    --derived_counter;

    {
        std::cout << "TEST - graham::shared_ptr<base> bp2 (new derived(), [] (derived* d){ delete d;});" << std::endl;

        graham::shared_ptr<base> bp2 (new derived(), [] (derived* d){ delete d;});
        bp2.reset();
        assert(derived_counter==0 && base_counter == 0);
    }

    {
        std::cout << "TEST - graham::shared_ptr<base> bp2 (new derived()); (no reset) " << std::endl;
        graham::shared_ptr<base> bp2 (new derived());
    }

    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - get deleter, expect null" << std::endl;
        graham::shared_ptr<base> bp (new base());
        using Deleter = decltype(dbase);
        Deleter* deleter = graham::get_deleter<Deleter, base>(bp);
        assert (deleter == nullptr);
        bp.reset();
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - get_deleter, expect non null " << std::endl;
        graham::shared_ptr<base> bp2 (new derived(), delit);
        assert(derived_counter==1 && base_counter == 1);
        using Deleter = decltype(dbase);
        Deleter* deleter = graham::get_deleter<Deleter, base>(bp2);
        assert(deleter != nullptr);
        bp2.reset();
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - alias ctor " << std::endl;
        base* bp2 = new base();
        base* bp3 = new base();
        {
            graham::shared_ptr<base> bsp (bp2 );
            graham::shared_ptr<base> bsp2 (bsp,bp3 );
        }
        assert(derived_counter==0 && base_counter == 1);
        delete bp3;
        assert(derived_counter==0 && base_counter == 0);
    }
    assert(derived_counter==0 && base_counter == 0);


    {
        std::cout << "TEST - multiple bp and assignment"  << std::endl;
        graham::shared_ptr<base> bp;
        graham::shared_ptr<base> bp2(bp);
        graham::shared_ptr<base> bp3(new base());
        bp = bp3;
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - equality" << std::endl;
        graham::shared_ptr<base> bp(new derived());
        graham::shared_ptr<base> bp2(bp);
        assert(bp == bp2);
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - nullptr " << std::endl;
        graham::shared_ptr<base> bp(nullptr);
        graham::shared_ptr<base> bp2(bp);
        assert(bp2.get() == nullptr);
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - equality (empty) " << std::endl;
        graham::shared_ptr<base> bp; 
        graham::shared_ptr<base> bp2(bp);
        assert(bp == bp2);
    }

    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - equality base/derived" << std::endl;
        graham::shared_ptr<derived> bp(new derived()); 
        graham::shared_ptr<base> bp2(bp);
        assert(bp == bp2);
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - invocation"  << std::endl;
        graham::shared_ptr<derived> bp(new derived(3)); 
        graham::shared_ptr<base> bp2;

        bp2 = std::move(bp);
        assert(bp2->_i == 3);

    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - operator!" << std::endl;
        graham::shared_ptr<derived> bp;
        assert(bp.get() == nullptr);

        graham::shared_ptr<derived> bp2(new derived());
        if (!bp2)
        {
            assert(false);
        }

    }

    {
        std::cout << "TEST - multiple instances & assignment" << std::endl;
        graham::shared_ptr<derived> bp(new derived());
        graham::shared_ptr<base> bp2;
        bp2 = bp;
        graham::shared_ptr<base> bp3(new derived());
        graham::shared_ptr<base> bp4(new derived());
        graham::shared_ptr<base> bp5(new derived());
        graham::shared_ptr<base> bp6(new derived());
        graham::shared_ptr<base> bp7(new base());
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - multiple instances & multiple assignment" << std::endl;
        graham::shared_ptr<derived> bp (new derived());
        graham::shared_ptr<base> bp1;
        graham::shared_ptr<base> bp2;
        graham::shared_ptr<base> bp3;
        graham::shared_ptr<base> bp4;
        bp1 = bp;
        bp2 = bp1;
        bp3 = bp2;
        bp4 = bp3;
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - multiple base and multiple derived " << std::endl;
        graham::shared_ptr<derived> dp (new derived());
        graham::shared_ptr<derived> dp2 (new derived());
        graham::shared_ptr<base> bp1(new base());
        graham::shared_ptr<base> bp2(new base());
        graham::shared_ptr<base> bp3(new base());
        graham::shared_ptr<base> bp4(new base());
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - swap and get value " << i++ << std::endl;
        graham::shared_ptr<base> bp(new base(123));
        graham::shared_ptr<base> bp2(new base(321));
        bp.swap(bp2);
        assert(bp->get() == 321);
        assert(bp2->get() == 123);
    }
    assert(derived_counter==0 && base_counter == 0);


    {
        std::cout << "TEST - get base value" << std::endl;
        {
            graham::shared_ptr<base> bp(new base(123));
            assert (bp->get() == 123);
            bp.reset();
        }
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - multiple reset" << std::endl;
        {
            graham::shared_ptr<base> bp(new base(123));
            bp.reset();
            bp.reset(); 
            bp.reset();
            bp.reset();
            bp.reset();
            bp.reset();
        }
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - unique() true" << std::endl;
        {
            graham::shared_ptr<base> bp(new base(123));
            assert(bp.unique() == true);
        }
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - unique() false " << std::endl;
        {
            graham::shared_ptr<base> bp(new base(123));
            graham::shared_ptr<base> bp2 = bp;
            assert(bp.unique() == false);
        }
    }
    assert(derived_counter==0 && base_counter == 0);

    {
        std::cout << "TEST - operator bool() true" << std::endl;
        graham::shared_ptr<base> bp{new base()};
        assert(bp);
    }

    {
        std::cout << "TEST - operator bool() false" << std::endl;
        graham::shared_ptr<base> bp;
        assert(!bp);
    }

    {
        std::cout << "TEST - operator<< " << std::endl;
        {
            graham::shared_ptr<base> bp(new derived(123));
            std::cout << bp << std::endl;

        }
    }

    return 0;

}
