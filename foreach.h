#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // forward

using namespace std;

template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it)
        func(*it, forward<Args>(args)...);
    // cout<<endl;
}

// Variadic templates: template <typename ...Args>
// Variadic templates allow a function or class to accept an arbitrary
// number of arguments.
// Example: template <typename ...Args> func() { // ... }
template <typename Iterator, typename Func, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it){
        if (func(*it, forward<Args>(args)...))
            return it;
    }
    return end;
}

template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}

#endif // __FOREACH_H__