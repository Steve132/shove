namespace shv{

    template<class T>
    struct as_class;

    template<class T>
        requires (!std::is_class_v<T>)
    struct as_class<T>{
        struct type{
        protected:
            T value;
        public:
            template<class ...Args>
            type(Args&& ...args):
                value(std::forward<Args>(args)...)
            {}

            template<class Arg>
            auto operator=(Arg&& a) {
                return value=std::forward<Arg>(a);
            }
    // Lvalue Conversion Operator
            operator T& () & {
                return value;
            }

            // Rvalue Conversion Operator
            operator T&& () && noexcept {
                return std::move(value);
            }

            // Const Lvalue Conversion Operator
            operator const T& () const & {
                return value;
            }

        };
    };

    template<class T>
        requires std::is_class_v<T>
    struct as_class<T>{
        using type=T;
    };

    template<class T>
    using as_class_t=typename as_class<T>::type;    
}
