
#include <functional>
#include <memory>

#include "noncopyable.h"

// one channel per thread?
class Channel {
    NONCOPYABLE(Channel)
public:
    Channel();
};