////////////////////////////////////////////////////////////////////////////////
// Example usage of CATCH_ALL(char*)

//#include <iostream>
//#include <string>
//#include "catch_all_exception.hpp"
//
//bool some_function (int i) {	
//
//	std::string stuff("asdf");
//	
//	try {
//		std::cout << stuff.at(i);
//	} 
//
//	CATCH_ALL("some_function");
//
//	return true;
//}
//
//int main()
//{
//	try {
//		some_function(120);
//	}
//	
//	catch (catch_all_exception e) {
//		std::cerr << e.what();
//	}
//	
//	return 0;
//}
//

///////////////////////////////////////////////////
//Displays at the time of writing this:

//Nov  1 2008 -- 18:12:32
//What: std::exception
//File: /home/jake/code/main.cpp
//Line: 57
//Hint: some_function
//		

#ifndef CATCH_ALL_EXCEPTION_HPP
#define CATCH_ALL_EXCEPTION_HPP

#include <stdexcept>
#include <string.h>

const char __ERROR_MESSAGE__[] = 
	"%s -- %s\nWhat: %s\nFile: %s\nLine: %u\nHint: %s\n";
	
const size_t ERROR_TAIL_SIZE = 30;

const size_t ERROR_MESSAGE_SIZE = 
	sizeof(__DATE__) + sizeof(__TIME__) + 
	sizeof(__FILE__) + sizeof(__LINE__) + 
	sizeof(__ERROR_MESSAGE__) + ERROR_TAIL_SIZE;

#define CATCH_ALL(hint) \
	catch (std::exception e) { \
		char errStr[ERROR_MESSAGE_SIZE]; \
		sprintf(errStr,__ERROR_MESSAGE__, \
			__DATE__, __TIME__, e.what(), __FILE__, __LINE__, hint); \
		\
		throw (catch_all_exception)(errStr); \
	} \
	\
	catch (...) { \
		char errStr[ERROR_MESSAGE_SIZE]; \
		sprintf(errStr,__ERROR_MESSAGE__, \
			__DATE__,__TIME__,"non-std exception",__FILE__,__LINE__,hint); \
			\
		throw (catch_all_exception)(errStr); \
	} 

class catch_all_exception : public std::exception {
	private:
		char ex_str[ERROR_MESSAGE_SIZE];
	
	public:
		
		catch_all_exception (char* message) {
			strncpy(ex_str, message, ERROR_MESSAGE_SIZE);
		}
	
		char const* what() const throw() { return ex_str; }
};

#endif
