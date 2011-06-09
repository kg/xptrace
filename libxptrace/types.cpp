#include "xptrace.h"

#include <math.h>

namespace xptrace {
    int string::compareTo (const string& rhs) const {
        auto result = strncmp(characters, rhs.characters, std::min(length, rhs.length));

        if (result == 0) {
            if (length < rhs.length)
                result = -1;
            else if (length > rhs.length)
                result = 1;
        }

        return result;
    }

    bool operator < (const string& lhs, const string& rhs) {
        return lhs.compareTo(rhs) < 0;
    }

    bool operator > (const string& lhs, const string& rhs) {
        return lhs.compareTo(rhs) > 0;
    }

    bool operator == (const string& lhs, const string& rhs) {
        return lhs.compareTo(rhs) == 0;
    }

    bool operator != (const string& lhs, const string& rhs) {
        return lhs.compareTo(rhs) != 0;
    }
}