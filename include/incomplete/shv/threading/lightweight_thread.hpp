

struct cthread{


    using id=size_t;
    std::function<void ()> task;
    
    template<class Callable,class ...Args>
    cthread(Callable&& call,Args&&... args):
        task(
            call,args...)){}
    
};

