# InitializableTypes
Some basic type definitions to annotate that a type is uninitialized or intiailized. The envisioned use case is for situations where not all fields of some data type are known at instantiation and must be filled in at a much later point in the code. By annotating the types as 'Initialized' or 'Uninitialized', one makes clear the status of the fields and help avoid accidental use of default values.

## Why Use Initializable Types?

When instantiating a struct or class in C++, RAII dictates that all fields must be given a valid value at this time, many times a default value. However, there are many cases where not all the values are known until long after the struct is instantiated. 

Using a default value is not desirable in these cases as it does not specify to the programmer that the contained value is default or the one intended. If the code is complicated enough, it is an easy logical error to use the default value because the real value was forgotten to be filled in. If this happens, it will produce no errors or complaints from the language, making finding the source of the error more difficult.

One could attempt to use other solutions like `std::optional` but these convey the wrong message about your data type to others reading your code. Many times these fields are in fact mandatory, but simply not known yet. 

As such, this library creates initializable types to aid empower the programmer in the aforementioned use cases with an aim to help pinpoint logic errors and to be intentional and clear to anyone using the code base.

## Setup, requirements, and basic use

To use the code, one only needs to download and include the header file `initializable.h` in your project. This project requires C++17 or later as it uses `std::variant` and `std::optional`. The unit test use [Catch2](https://github.com/catchorg/Catch2), a header only library which is already included in the `tests/` sub-directory.

Given a type `Type` that you wish to make initiazable, it is advised to add the following type aliases:

``` 
template<Status T>
using YourPreferredName = InitializableStruct<T, Type>;
typedef Initializable<Type> AnotherPreferredName;
```
The first type alias gives you access to two phantom types: `YourPreferredType<UnInitialized>` and `YourPreferredType<Initialized>`. The type `AnotherPreferredName` is equivalent to `std::variant<YourPreferredType<UnInitialized>, YourPreferredType<Initialized>>` and is the main initializable type. This is anticipated to be the type of the field that needs initializing to / from which the two phantom types can be assigned / extracted.

The type `YourPreferredType<UnInitialized>` is actually an empty struct while `YourPreferredType<Initialized>` has a single field of type `Type` called `value` which will store your initialized value once it is known.

An intializable type has no default constructor by design, instead favoring explicit instantiation. It is generally expected that initial declarations be of the form 
``` AnotherPreferredName = YourPreferredName<UnInitialized>();```

## Included helper functions

Added to the `std` namespace are the following functions to aid in the manipulation of your types in a safe and clear fashion.

`void std::initialize<Type>(AnotherPreferredName UninitializedVariable, Type value)` : This takes in your initializable variable and changes the internal variant to be of type `YourPreferredName<Initialized>` holding the specified value.

`Type std::extract<Type>(InitializedVariable)` : This takes either a variable of type `YourPreferredName<Initialized>` or `AnotherPreferredName` and extracts the contained value (assuming there is one).

`YourPreferredName<Initialized>& std::get_initialized<Type>(AnotherPreferredName variable, std::optional<Type> value)` : This method is intended to get access to the initialized variable contained in the underlying variant type `AnotherPreferredName`. This method may simultaneously initialize and return the inner initialized type or simply retrieve it.

## Exceptions

The most important exception is `std::extract_uninitialized_value` which is thrown when you attempt to get a value from a variable that has not been initialized yet. This is thrown by both the `std::extract` and `std::get_initialized` methods.

Furthermore, in many cases it is likely an error if an initialized variable is initialized a second time. By default, this will not produce and exception but rather log a warning to `std::cerr`. If one wishes to change the value of an initialized variable, the following three patterns are encouraged instead:
```
AnotherPreferredName initializable = ...;
// Pattern 1.
YourPreferredName<Initialized> initialized_variable = std::get_initialized<Type>(initializable);
initialized_variable = new_value    // new_value may have type Type. The = operator has been overloaded.

// Pattern 2.
initializable = YourPreferredName<Initialized>(new_value);    // again new_value should have type Type.

// Pattern 3. (Recommended)
std::get_initialized<Type>(initializable) = new_value;  // Again using the overloaded = operator and the fact that std::get_initialized returns a reference.
```

However, you may still use `std::initialize` and `std::get_initialized` to change the internal value of an already initialized variable if so desired. There is a type trait called `InitializableTraits` that contains the re-initialization policy. By specializing the trait to your defined type, you may defined the policy to allow re-initializations silenty (no warnings logged), warning (the default, as explained above), or error. 

If the policy is set to error, then re-initializing an already initialized variable will throw an `std::re_initialize_error` exception. This is intended for when strict enforcement of re-initialization is desired to catch logic errors.

The following demonstrates how to change the policy.
```
// Defined already in initializable.h, included here for clarity.
enum ReInitializationPolicy {
  WARNING,
  ERROR,
  SILENT
};

template<>
InitializableTraits<AnotherPreferredName>{
    static const ReInitializationPolicy re_initialization_policy = ...; // Specify the desired policy here; default is WARNING.
}
```

## Examples

Please see the examples (in `example.cpp`) and unit tests (in `tests/test_initializable_types.h`). The first can be compiled from the project root via `make example` and the latter similarly by running `make test`.
