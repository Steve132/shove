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
        };
    }

    template<class T>
    inline const bool is_optional_v=is_optional<T>::value;

    namespace concepts{

    }
    
}


#endif