#pragma once

/*
Copyright 2024 Steven Braeger

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include<source_location>
#include<string>
#include<string_view>
#include<cstdlib>

namespace shv{
namespace detail{
    namespace static_type_info
    
    static constexpr const std::string_view static_type_info_parse_location(const std::string_view&) noexcept;
    
    template<class Tmatch>
    struct static_type_info_impl
    {
    public:
        static constexpr const std::string_view get_location() noexcept;
    };
}

struct identifier_t{
protected:
    std::string val;
public:

};

template<class T>
struct static_type_info{
    public:
    static const constexpr std::string_view identifier=
        detail::static_type_info_impl<std::remove_cvref_t<T>>::get_location();
};

//impl
namespace detail{

    constexpr inline const std::string_view 
    static_type_info_parse_location(const std::string_view& full_view) noexcept
    {
        std::string_view new_view=full_view;
        #ifdef _MSC_VER
            const std::string_view prefix="static_type_info_impl<";
            new_view.remove_prefix(new_view.rfind(prefix)+prefix.length());
            new_view.remove_suffix(new_view.length()-new_view.find(">::get_location"));
            size_t first_position=new_view.find_first_of(" ");
            if(first_position != std::string_view::npos){
                first_position=new_view.find_first_not_of(" ",first_position);
                new_view.remove_prefix(first_position);
            }
            while(new_view.back()==' ') new_view.remove_suffix(1);

            return new_view;
        #else
            const std::string_view prefix="Tmatch";
            new_view.remove_prefix(new_view.rfind(prefix)+prefix.length());
            new_view.remove_prefix(new_view.find_first_not_of(" ="));
            new_view.remove_suffix(new_view.length()-new_view.find_first_of("];")); //gcc,clang
            while(new_view.back()==' ') new_view.remove_suffix(1);
            return new_view;
        #endif
    }
    
    template<class Tmatch>
    inline constexpr const std::string_view 
    static_type_info_impl<Tmatch>::get_location() noexcept
    {
        const std::source_location location =
               std::source_location::current();
        
        std::string_view full_view=location.function_name();
        
        return static_type_info_parse_location(full_view);
    }
}
}