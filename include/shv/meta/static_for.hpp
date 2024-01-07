#pragma once

#include<utility>

namespace shv{
    

/*
template <typename T, T Begin,  class Func, std::size_t ...Is>
            static constexpr void static_for_impl( Func &&f, std::index_sequence<Is...> )
            {
                ( f( std::integral_constant<std::size_t, Begin + Is>{ } ),... );
            }

            template <typename T, T Begin, T End, class Func >
            static constexpr void static_for( Func &&f )
            {
                static_for_impl<T, Begin>( std::forward<Func>(f), std::make_index_sequence<End - Begin>{ } );
            }*/

}