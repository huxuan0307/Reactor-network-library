#ifndef _BASE_LOGGING_H_
#define _BASE_LOGGING_H_

#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <sstream>
#include <unistd.h>
#include "noncopyable.h"
#include "LogDebug.h"

class Debug_inner;

extern std::string MessageLogger_file;
const std::string MessageLogger_type[5]{ "I","D","W","C","F" };


class MessageLogContext
{
	NONCOPYABLE (MessageLogContext)
public:
	constexpr MessageLogContext () :file{ nullptr }, function{ nullptr }, category{} { };
	constexpr MessageLogContext (const char* fileName, const char* functionName, const char* categoryName)
		: file{ fileName }, function{ functionName }, category{ categoryName }
	{}
	const char* file;
	const char* function;
	const char* category;
private:
	friend class MessageLogger;
	friend class Debug_inner;
};

class MessageLogger
{
	NONCOPYABLE (MessageLogger)
public:
	constexpr MessageLogger () : context () { }
	constexpr MessageLogger (const char* file, const char* function)
		: context (file, function, "default")
	{ }
	constexpr MessageLogger (const char* file, const char* function, const char* category)
		: context (file, function, category)
	{ }
	
	Debug_inner debug ()const;
	Debug_inner info ()const;
	Debug_inner warning ()const;
	Debug_inner critical ()const;
	Debug_inner fatal ()const;

	static void setLogFile (std::string file) { MessageLogger_file = file; }
	static void setLogPostfix (std::string postfix);
private:
	MessageLogContext context;

	friend class Debug_inner;

};

#define Debug MessageLogger().debug
#define Info MessageLogger().info
#define Warning MessageLogger().warning
#define Critical MessageLogger().critical
#define Fatal MessageLogger().fatal
#define CRITICAL Critical() << "[" << __FILE__ << ":" << __LINE__ << "]"
#define WARNING Warning() << "[" << __FILE__ << ":" << __LINE__ << "]"
#define FATAL Fatal() << "[" << __FILE__ << ":" << __LINE__ << "]"
#define TRACE Debug() << "[" << __FILE__ << ":" << __LINE__ << "]"
std::string bitstreamToString(const std::string&s);
std::string bitstreamToString(const char* s);

#endif