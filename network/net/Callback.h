#ifndef _CALLBACK_H_
#define _CALLBACK_H_
#include <functional>
using std::function;
using TimerCallback = function<void()>;
#endif