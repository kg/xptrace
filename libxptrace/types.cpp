/*
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Program Tracing and Instrumentation Toolkit.

The Initial Developer of the Original Code is Mozilla Corporation.

Original Author: Kevin Gadd (kevin.gadd@gmail.com)
*/

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

    void string::operator = (const string& rhs) {
        characters = rhs.characters;
        length = rhs.length;
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