#ifndef SHV_META_OPTIONAL_HPP
#define SHV_META_OPTIONAL_HPP

#include<optional>

namespace shv
{
    namespace {
        template<class T>
        struct is_optional{
            static constexpr bool value=false; 
        };

        template<class T>
        struct is_optional<std::optional<T>> {
            static constexpr bool value=true;
            using type=T;
        };
    }

    template<class T>
    inline constexpr bool is_optional_v=is_optional<T>::value;

    template<class T>
    using optional_result=typename is_optional<T>::type;
    
    namespace concepts{
        template<class T>
        concept optional=is_optional_v<T>;
    }
    
}


#endif