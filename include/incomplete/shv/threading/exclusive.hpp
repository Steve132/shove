#include<vector>
#include<mutex>
#include<shared_mutex>

template<class T,class LG>
struct guard;


struct exclusive_traits{
    template<class T>
    static auto make_mutex(T&& a);

    template<class Mutex>
    static auto make_readonly_lock_guard(Mutex&& m);

    template<class Mutex>
    static auto make_lock_guard(Mutex&& m);
};

template<class T,class Traits>
struct exclusive_base_order{
protected:
    using mutex_type=decltype(Traits::make_mutex(std::declval<T>()));
    mutex_type m;
    void init(T&& t){
        m=Traits::make_mutex(std::forward<T>(t));
    }
};

/**/
template<class T,class Traits=exclusive_traits>
struct exclusive: protected exclusive_base_order<T,Traits>
{
//protected:
    T self;
public:
    template<class ...Args>
    exclusive(Args&& ...args):self(std::forward<Args>(args)...)
    {
        init(self);
    }

    template<class OT,class OTraits>
    exclusive(exclusive<OT,OTraits>&& oe);

    template<class OT,class OTraits>
    friend struct exclusive;
};


template<>
auto exclusive_traits::make_mutex<int>(int&& t){
    return std::mutex{};
}


template<class Mutex>
inline auto exclusive_traits::make_readonly_lock_guard(Mutex&& m){
    return make_lock_guard(std::forward<Mutex>(m));
}

template<class Mutex>
inline auto exclusive_traits::make_lock_guard(Mutex&& m){
    return std::lock_guard<Mutex>(std::forward<Mutex>(m));
}

template<class LG>
struct guard_order_base
{
protected:
    LG lg;
};

template<class T,class LG>
struct guard: public guard_order_base<LG>
{
protected:
    using rref_t=std::add_rvalue_reference_t<T>; 
    rref_t self;
public:
    template<typename E>
    guard(E&& other);


};

int compute(const exclusive<int>& r){
    guard<int,std::shared_lock<std::shared_mutex>>  g(r);
    return 0;
}


int test(){
    return 0;    
}


#include<vector>
#include<atomic>
#include<thread>

template<class T,class LG>
struct guard;

struct exclusive_base{
    using tid_t=size_t;
    std::atomic<tid_t> current_owner;
    std::atomic<size_t> num_reading_threads;
    
    static tid_t this_thread_id() {
        static thread_local tid_t v=std::hash<std::thread::id>{}(
            std::this_thread::get_id());
        return v;
    }
    static tid_t none() {
        std::thread::id none_tid;
        tid_t v=std::hash<std::thread::id>{}(none_tid);
        return v;
    }
    static tid_t reader() { return ~none(); }

    //a.cex(b,c) <=> if(a == b) { a = c; return true; } else { b = a; return false }

    exclusive_base(){    

    }
    void lock(){
        tid_t zer=none();
        tid_t t=this_thread_id();
        
        
    }

};


template<class T>
struct exclusive{
public:
        T value;
public:
    template<class ...Args>
    exclusive(Args&&... args):
        value(std::forward<Args>(args)...)
    {
        
    }
};



exclusive_base get_default(){
    return exclusive_base();
};

#include<iostream>
int main(int argc,char** argv){
    
    return 0;
}




template<class T,class LG>
struct guard;


template<class BaseMutex>
struct ensure_rw_adapter;

template<class BaseMutex>
    requires (requires(BaseMutex&& bm){
        {bm.lock_shared()};
        {bm.try_lock_shared()};
        {bm.unlock_shared()};
    })
struct ensure_rw_adapter<BaseMutex>: public BaseMutex{
    using BaseMutex::lock;
    using BaseMutex::try_lock;
    using BaseMutex::unlock;
    using BaseMutex::lock_shared;
    using BaseMutex::try_lock_shared;
    using BaseMutex::unlock_shared;
};

template<class BaseMutex>
struct ensure_rw_adapter<BaseMutex>: public BaseMutex{
    using BaseMutex::lock;
    using BaseMutex::try_lock;
    using BaseMutex::unlock;
    decltype(auto) lock_shared(){
        return BaseMutex::lock();
    }
    decltype(auto) try_lock_shared(){
        return BaseMutex::lock();
    }
    decltype(auto) unlock_shared(){
        return BaseMutex::lock();
    }
};

template<class BaseMutex>
struct write_lock_init_adapter: 
    public BaseMutex
{
protected:
    template<class ...Args>
    write_lock_init_adapter(Args&& ...args):
        BaseMutex(std::forward<Args>(args)...)
    {
        BaseMutex::lock();
    }

    ~write_lock_init_adapter(){
        BaseMutex::unlock();
    }
};

template<class T>
struct exclusive_default_base_mutex: public std::shared_mutex
{};

template<class T,class base_mutex>
struct exclusive_computed_mutex: public base_mutex
{};

template<class Arg>
concept BasicLockable=requires(Arg&& a){
    {a.lock()};
    {a.unlock()};
};

template<class Arg>
    requires !(BasicLockable<Arg>)
template<class Arg>
void make_lock(Arg&& a):


template<class...Args>
void lock_all()

struct multilock_call{

    template
    multilock_call()


};


#include<array>

struct bigdata{
    std::array<uint32_t,64> data;
};

/*
void increment_part(exclusive<bigdata>& x,int i){
    x.data[i]++;
}
*/


Also use a binary semaphore to own and de-own the mutex.


template<class Arg>
    requires (!BasicLockable<Arg>)
decltype(auto) make_lock(Arg&& a){
    return null_lock{};
};

template<class Arg>
    requires (BasicLockable<Arg>)
decltype(auto) make_lock(Arg&& a){
    return std::forward<Arg>(a);
}

template<class Arg>
concept LockWrapper= BasicLockable<Arg> && requires(Arg&& a){
    std::forward<Arg>(a).
}

//deadlock avoidance: based on lockwrapper recursively find the actual mutex object
//then set all the real mutexes which are not owned (not wrappers) to a set 


template<class...Args>
void lock_all(Args&&... args){
    std::lock(make_lock(std::forward<Args>(args))...);
}

using mut=std::mutex;
using smut=std::shared_mutex;
using lg=std::unique_lock<std::shared_mutex>;
using slg=std::shared_lock<std::shared_mutex>;

void lock_test(mut& m1,mut& m2,smut& sm1,lg& l1,slg& s1){
    lock_all(m1,m2,sm1,l1,s1);
}




#include<shared_mutex>
#include<atomic>
#include<mutex>
#include<concepts>
#include<semaphore>
#include<iostream>

void print(const char* s){
    std::cout << s << std::endl;
}


template<std::ptrdiff_t count>
struct semaphore_lock: protected std::counting_semaphore<count>
{
protected:
    using base=std::counting_semaphore<count>;
public:
    using base::base;

    void lock(){
        print("slock{");
        std::binary_semaphore::acquire();
        print("}slock");
    }
    void unlock(){
        print("sunlock{");
        std::binary_semaphore::release();
        print("}sunlock");
    }
    ~semaphore_lock(){
        unlock();
    }
};

//block until its not zero.
//on init, it's this;
//on read lock, try to take the token.  If it's not 0, then take what's there
//and 
//on read unlock, 


/*
T B |
$ $ | $
A $ | $<-A

//swap TOP and  
*/



template<class T>
struct exclusive: public semaphore_lock<1>{
protected:
    T value;
public:
    template<class ...Args>
    exclusive(Args&&... args):
        semaphore_lock<1>(0),
        value(std::forward<Args>(args)...)
    {
        unlock();
    }
    ~exclusive(){
        lock(); //this is swapped so that in the destructor after the variable
                //is actually locked during that scope.
    }

    
    struct guard{
        T&& rvalue_ref;
        guard(T t){}
        guard(T&& t):rvalue_ref(std::forward<T>(t))
        {}
        guard(){
            print("guard{}");
        }
        ~guard(){
            print("~guard{}");
        }
        operator T () {
            return rvalue_ref;
        }
    };
    operator guard&& () & {
        return guard(std::forward<T>(value));
    }
};


#include<iostream>

struct M{
    int a=0;
    double z=0.0;
};




void do_object(M& out,M a){
    print("do{");
    out=a;
    print("}do");
}


void exclusive_test(M& out,exclusive<M>& a){
    do_object(out,a);
}

int main(int argc,char** argv){
    M tom;
    exclusive<M> bob;
    exclusive_test(tom,bob);
    return 0;
} 