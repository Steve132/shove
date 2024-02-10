#pragma once

#include<utility>

#ifndef PROCESS_SINGLETON_IS_DLL

#ifdef PROCESS_SINGLETON_IS_DLL

#ifdef _WIN32
#define PROCESS_SINGLETON_IMPORT_MAIN __declspec(dllimport)
#define PROCESS_SINGLETON_EXPORT_MAIN __declspec(dllexport)
#else
#define PROCESS_SINGLETON_IMPORT_MAIN 
#define PROCESS_SINGLETON_EXPORT_MAIN 
#endif

namespace shv{


template<class T,class SingletonStorage=T>
struct singleton{
protected:
    template<class T>
    static SingletonStorage& PROCESS_SINGLETON_IMPORT_MAIN derived_instance(){
        static SingletonStorage value;
        return value;
    }
public:
    static T& PROCESS_SINGLETON_IMPORT_MAIN instance(){
        static thread_local T& value=derived_instance.instance();
        return value;
    }
};


};

https://gcc.gnu.org/wiki/Visibility


//hi

#define P_MAIN_ONLY

//on linux do we need to explicitly suppress the definition?
//according to links, shared objects automatically resolve their 
//global vars on load.
//there's explicit visibility support tho to make this better.
//https://gcc.gnu.org/wiki/Visibility

#ifdef _WIN32 //if it's a dll platform.
    #ifndef P_IS_DLL //try to autodetect
        #ifndef P_SUPPRESS_AUTODETECT_WARNING
            #warn "Singleton on windows in the presence of possible DLL.  Attempting to autodetect dll build"
        #endif

        #if defined(_MSC_VER)
            #ifdef _DLL
                #define IS_DLL true
            #else
                #define IS_DLL false
            #endif
        #else //clang and gcc 

    #if IS_DLL
        #define P_MAIN_ONLY __declspec(dllimport)
    #else 
        #define P_MAIN_ONLY __declspec(dllexport)
    #endif 
#endif


//idea: extern __declspec(dllexport) is the same as __declspec(dllimport)
//but no implied redefinition of the declaration so no preprocessor shit is needed
//a static member is implied extern.
//https://learn.microsoft.com/en-us/cpp/c-language/definitions-and-declarations-c?view=msvc-170


//so yeah.  


inline int P_MAIN_ONLY bob(int& i){
    return i+1;
}

int tom(int& i){
    return bob(i);
}



template<class T>
inline __declspec(dllimport) T& singleton(){
    static T s;
    return s;
}


int test(int& i){
    return i+singleton<int>();
}


template __declspec(dllexport) int& singleton<int>();