#include <string>
#include <iostream>
#include <initializable.h>

typedef std::pair<std::string, std::string> InnerType;
template <Status T>
using StringPair = InitializableStruct<T, InnerType>;
typedef Initializable<InnerType> InitializableStringPair;

template <Status T>
using Age = InitializableStruct<T, unsigned>;
typedef Initializable<unsigned> InitializableAge;

struct Card {
	std::string sender;
	std::string receiver;
	InitializableStringPair message = StringPair<UnInitialized>();
};

int main(){
	Card birthday_card = Card{.sender="Mom", .receiver="Son"};

	// Example showing that accessing an uninitialized value throws exception
	try{
		std::extract(birthday_card.message);	
	} catch (std::extract_uninitialized_value &e){
		std::cout << e.what() << std::endl;
	} 
	// Initialize the variable
	std::initialize<InnerType>(birthday_card.message, std::make_pair("Harpy", "Birthday!"));

	// Re-initialize, receive warning.
	std::initialize<InnerType>(birthday_card.message, std::make_pair("Happy", "Birthday!"));

	std::get_initialized(birthday_card.message) = std::make_pair("HELLO", "WORLD");

	// Now the variable can be extracted
	std::cout << std::extract(birthday_card.message).first
			  << " " 
			  << std::extract(birthday_card.message).second
			  << std::endl;
	
	return 0;
}