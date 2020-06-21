#ifndef _INITIALIZABLE_H_
#define _INITIALIZABLE_H_

#include 
enum Status {
  UnInitialized,
  Initialized
};

template <Status T, class Inner>
struct InitializableStruct {};

template <class Inner>
struct InitializableStruct<Initialized, Inner>{
    Inner value;
};

template <class Inner>
using Initializable = std::variant<InitializableStruct<UnInitialized, Inner>, InitializableStruct<Initialized, Inner>>;

namespace std {
    template <class Inner>
    InitializableStruct<Initialized, Inner> initialize(Initializable<Inner> uninitialized, Inner value){
        if (std::holds_alternative<InitializableStruct<UnInitialized,Inner>>(uninitialized)){
            return InitializableStruct<Initialized, Inner>{.value=value};
        }
        return std::get<InitializableStruct<Initialized, Inner>>(uninitialized);
    }

    template <class Inner>
    Inner extract(Initializable<Inner> initialized) {
        return std::get<InitializableStruct<Initialized, Inner>>(initialized).value;
    }
};

#endif