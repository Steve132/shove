#pragma once

#include <optional>
#include <istream>
#include <ostream>

namespace std{

template<class T>
inline ostream& operator<<(ostream& out,const optional<T>& t){
    if(t){
        out << *t;
    }
    else{
        out << "null";
    }
    return out;
}
template<class T>
inline istream& operator>>(istream& in,optional<T>& t){
    std::istream::sentry sentry(in); // Ensure the stream is ready
    if (!sentry) {
        t = std::nullopt; // Stream is in a bad state
        return in;
    }
    char token[5]={};
    // Save the stream's current state
    auto stream_pos = in.tellg();
    auto stream_state = in.rdstate();
    const char* cmptok="null";


    if (in >> token && strncmp(token,cmptok,5)==0) {
        t = std::nullopt; // Successfully parsed "null"
        return in;
    }

    // Restore the stream state if "null" was not found
    in.clear(stream_state);
    in.seekg(stream_pos);

    // Try to parse the object
    T value;
    if (in >> value) {
        t = std::move(value); // Successfully parsed the object
    } else {
        t = std::nullopt; // Object parsing failed
    }

    return in;
}


}
