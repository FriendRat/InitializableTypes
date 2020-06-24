# InitializableTypes
Some basic type definitions to annotate that a type is uninitialized or intiailized. The envisioned use case is for situations where not all fields of some data type are known at instantiation and must be filled in at a much later point in the code. By annotating the types as 'Initialized' or 'Uninitialized', one makes clear the status of the fields and help avoid accidental use of default values.

## Why Use Initializable Types?

When instantiating a struct or class in C++, RAII dictates that all fields must be given a valid value at this time, many times a default value. However, there are many cases where not all the values are known until long after the struct is instantiated. 

Using a default value is not desirable in these cases as it does not specify to the programmer that the contained value is default or the one intended. If the code is complicated enough, it is an easy logical error to use the default value because the real value was forgotten to be filled in. If this happens, it will produce no errors or complaints from the language, making finding the source of the error more difficult.

One could attempt to use other solutions like `std::optional` but these convey the wrong message about your data type to others reading your code. Many times these fields are in fact mandatory, but simply not known yet. 

As such, this library creates initializable types to aid empower the programmer in the aforementioned use cases with an aim to help pinpoint logic errors and to be intentional and clear to anyone using the code base.

## Setup and basic use

To use the code, one only needs to download and include the header file `initializable.h` in your project. Given a type `Type` that you wish to make initiazable, it is advised to add the following type aliases:

``` 
template<Status T>
using YourPreferredName = InitializableStruct<T, Type>;
typedef Initializable<Type> AnotherPreferredName;
```
The first type alias gives you access to two phantom types: `YourPreferredType<UnInitialized>` and `YourPreferredType<Initialized>`. The type `AnotherPreferredName` is equivalent to `std::variant<YourPreferredType<UnInitialized>, YourPreferredType<Initialized>>`. This is anticipated to be the type of the field that needs initializing to / from which the two phantom types can be assigned / extracted.

The type `YourPreferredType<UnInitialized>` is actually an empty struct while `YourPreferredType<Initialized>` has a single field of type `Type` called `value` which will store your initialized value once it is known.

## Included helper functions

Added to the `std` namespace are the following functions to aid in the manipulation of your types in a safe and clear fashion.


