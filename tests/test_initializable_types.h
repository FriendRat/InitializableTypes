#include <initializable.h>
#include <stdexcept>

template <Status T>
using Uint = InitializableStruct<T, unsigned>;
typedef Initializable<unsigned> InitializableUint;


TEST_CASE("Test basic", "[basic]") {

	SECTION("Test is_intialized"){
		InitializableUint init_uint = Uint<UnInitialized>();
		REQUIRE(!is_initialized(init_uint));

		// initialize via std::intialize
		std::initialize<unsigned>(init_uint, 1);
		REQUIRE(is_initialized(init_uint));

		init_uint = Uint<UnInitialized>();
		REQUIRE(!is_initialized(init_uint));

		// initialize via std::get_initialized
		std::get_initialized<unsigned>(init_uint, 1);
		REQUIRE(is_initialized(init_uint));
	}

	SECTION("Test initialize value "){
		InitializableUint init_uint = Uint<UnInitialized>();
		std::initialize<unsigned>(init_uint, 1);
		REQUIRE(std::extract<unsigned>(init_uint) == 1);
	}

	SECTION("Test extract with initialized variable"){
		InitializableUint init_uint = Uint<Initialized>(1);
		REQUIRE(std::extract<unsigned>(init_uint) == 1);
	}

	SECTION("Test get_initialized") {
		InitializableUint init_uint = Uint<UnInitialized>();
		Uint<Initialized>& inner = std::get_initialized<unsigned>(init_uint, 1);
		// Test that values are correct
		REQUIRE(inner.value == 1);
		REQUIRE(std::extract<unsigned>(inner) == 1);

		// The assignemt operator works on initialized variables
		inner = 2;
		// Test that references work as expected
		Uint<Initialized> another_inner = std::get_initialized<unsigned>(init_uint);
		REQUIRE(another_inner.value == 2);
	}

	SECTION("Test get_initialized + assignment"){
		InitializableUint init_uint = Uint<Initialized>(2);
		std::get_initialized<unsigned>(init_uint) = 1;
		REQUIRE(std::extract<unsigned>(init_uint) == 1);
	}
}

template <Status T>
using String = InitializableStruct<T, std::string>;
typedef Initializable<std::string> InitializableString;
template<>
struct InitializableTraits<InitializableString> {
	static const ReInitializationPolicy re_initialization_policy = ERROR;
};

template <Status T>
using Bool = InitializableStruct<T, bool>;
typedef Initializable<bool> InitializableBool;
template<>
struct InitializableTraits<InitializableBool> {
	static const ReInitializationPolicy re_initialization_policy = SILENT;
};

TEST_CASE("Test exceptions", "[exceptions]"){

	SECTION("Test re-initialize, warning policy"){
		InitializableUint init_uint = Uint<Initialized>(1);
		std::initialize<unsigned>(init_uint, 2);
		REQUIRE(std::extract<unsigned>(init_uint) == 2);
	}

	SECTION("Test re-initialize, silent policy"){
		InitializableBool init_bool = Bool<Initialized>(false);
		std::initialize<bool>(init_bool, true);
		REQUIRE(std::extract<bool>(init_bool));
	}

	SECTION("Test re_initialize, error policy"){
		InitializableString init_string = String<Initialized>("test");
		try {
			std::initialize<std::string>(init_string, "fail");
			REQUIRE(false);
		} catch (std::re_initialize_error& e) {
			std::string message(e.what());
			REQUIRE(message.compare("Tried to re-initialize a variable that was already initialized. Use assignment ( = ) instead.") == 0);
			REQUIRE(std::extract<std::string>(init_string).compare("test") == 0);
		}
	}

	SECTION("Test extract unitialized variable"){
		InitializableUint un_init = Uint<UnInitialized>();
		try {
			std::extract<unsigned>(un_init);
		} catch (std::extract_uninitialized_value& e) {
			std::string message(e.what());
			REQUIRE(message.compare("Tried to extract an uninitialized value from Initializable") == 0);
		}
	}

	SECTION("Test get_initialized no value"){
		InitializableUint un_init = Uint<UnInitialized>();
		try {
			std::get_initialized<unsigned>(un_init);
		} catch (std::extract_uninitialized_value& e) {
			std::string message(e.what());
			REQUIRE(message.compare("Tried to extract an uninitialized value from Initializable") == 0);
		}
	}

	SECTION("Test get_initialized, warning policy") {
		InitializableUint init_uint = Uint<Initialized>(1);
		Uint<Initialized> initialized = std::get_initialized<unsigned>(init_uint, 2);
		REQUIRE(std::extract<unsigned>(initialized) == 2);
		REQUIRE(std::extract<unsigned>(init_uint) == 2);	
	}
	
	SECTION("Test get_initialized, silent policy"){
		InitializableBool init_bool = Bool<Initialized>(false);
		Bool<Initialized> initialized = std::get_initialized<bool>(init_bool, true);
		REQUIRE(std::extract<bool>(initialized));
		REQUIRE(std::extract<bool>(init_bool));
	}

	SECTION("Test get_initialized, error policy") {
		InitializableString init_string = String<Initialized>("test");
		try {
			std::get_initialized<std::string>(init_string, "fail");
		} catch (std::re_initialize_error& e) {
			std::string message(e.what());
			REQUIRE(message.compare("Tried to re-initialize a variable that was already initialized. Use assignment ( = ) instead.") == 0);
			REQUIRE(std::extract<std::string>(init_string).compare("test") == 0);
		}
	}
}