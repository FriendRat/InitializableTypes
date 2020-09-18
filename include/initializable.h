#ifndef _INITIALIZABLE_H_
#define _INITIALIZABLE_H_
#include <variant>
#include <optional>
#include <iostream>

/*****************************************************************
    These define the type templates for the initializable type
******************************************************************/


// Denotes whether the variable has been initialized or not
enum Status {
  UnInitialized,
  Initialized
};

// A phantom type that wraps the variable we want to be initializable
template <Status T, class Inner>
struct InitializableStruct {};

// If the variable is initialized, we give it a field to store its value
template <class Inner>
struct InitializableStruct<Initialized, Inner>{
    Inner value;
    void operator = (const Inner val){value=val;};
    InitializableStruct(Inner value): value(value){};
    
};

// The actual type definition for an initializable variable
template <class Inner>
using Initializable = std::variant<InitializableStruct<UnInitialized, Inner>, InitializableStruct<Initialized, Inner>>;



/*****************************************************************
    These define traits for Initializable types to modify 
    certain behaviors
******************************************************************/

enum ReInitializationPolicy {
    WARNING,
    ERROR,
    SILENT
};

template <class Inner> struct InitializableTraits;

template <class Inner>
struct InitializableTraits<Initializable<Inner>> {
    static const ReInitializationPolicy re_initialization_policy = WARNING; 
    
};

/*******************************************************************
    These are useful methods for handling initializable types
********************************************************************/
namespace std {
    // Custom exception if trying to access value of unintialized variable
    struct extract_uninitialized_value : exception {
        const char * what () const throw () {
            return "Tried to extract an uninitialized value from Initializable";
        }
    };

    struct re_initialize_error : exception {
        const char* what () const throw () {
            return "Tried to re-initialize a variable that was already initialized. Use assignment ( = ) instead.";
        }
    };
    
    template <class Inner>
    bool is_initialized(const Initializable<Inner>& initializable){
        return std::holds_alternative<InitializableStruct<Initialized, Inner>>(initializable);
    }

    // Initializes the type with the value. If already initialized, this is overwrites the value.
    template <class Inner>
    void initialize(Initializable<Inner>& uninitialized, Inner& value){
        switch (InitializableTraits<Initializable<Inner>>::re_initialization_policy){
            case SILENT:
                uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                break;
            case WARNING:
                if (is_initialized(uninitialized)){
                    std::cerr << "WARNING: Re-initializing an already initialized variable. Use assignmet ( = ) instead." << std::endl;    
                }
                uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                break; 
            case ERROR:
                if (is_initialized(uninitialized)){
                    throw re_initialize_error();    
                } else {
                    uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                }                
        }
    }

    template <class Inner>
    void initialize(Initializable<Inner>& uninitialized, Inner&& value){
        switch (InitializableTraits<Initializable<Inner>>::re_initialization_policy){
            case SILENT:
                uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                break;
            case WARNING:
                if (is_initialized(uninitialized)){
                    std::cerr << "WARNING: Re-initializing an already initialized variable. Use assignmet ( = ) instead." << std::endl;    
                }
                uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                break; 
            case ERROR:
                if (is_initialized(uninitialized)){
                    throw re_initialize_error();    
                } else {
                    uninitialized = InitializableStruct<Initialized, Inner>{.value=value};
                }                
        }   
    }


    // Retrieves the value of the initialized variable. Throws exception if variable is unitialized
    template <class Inner>
    Inner extract(const InitializableStruct<Initialized, Inner>& initialized) {
        return initialized.value;
    }

    template <class Inner>
    Inner extract(const Initializable<Inner> initialized) {
        if (!std::holds_alternative<InitializableStruct<Initialized, Inner>>(initialized)){
            throw extract_uninitialized_value();
        }
        return std::get<InitializableStruct<Initialized, Inner>>(initialized).value;
    }

    // Initializes and returns the initialized variable
    template <class Inner>
    InitializableStruct<Initialized, Inner>& get_initialized(Initializable<Inner> &initializable, std::optional<Inner> value = {}) {
        if (!value && !is_initialized(initializable)) {
            throw extract_uninitialized_value();
        } else if (value) {
            initialize<Inner>(initializable, value.value());   
        }
        return std::get<InitializableStruct<Initialized, Inner>>(initializable);
    }
};

template <class Inner>
bool operator!(const Initializable<Inner> &initializable) {
    return !std::is_initialized<Inner>(initializable);
}

#endif