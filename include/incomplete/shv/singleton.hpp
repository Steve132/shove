#pragma once

//this works because extern dllexport is the same as dllimport so we can use
//it correctly, and a static data member of a class is extern implicitly
//simply define the instance in the cpp file for the TU you want to define it in.

//T singleton<T>::instance;

namespace shv{

#if _WIN32
#define DLLONE __declspec(dllexport)
#endif

template<class T>
struct singleton{
public:
    static DLLONE T main;
};

};

//https://gcc.gnu.org/wiki/Visibility



//idea: extern __declspec(dllexport) is the same as __declspec(dllimport)
//but no implied redefinition of the declaration so no preprocessor shit is needed
//a static member is implied extern.
//https://learn.microsoft.com/en-us/cpp/c-language/definitions-and-declarations-c?view=msvc-170
