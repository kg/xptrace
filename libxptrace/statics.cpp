#include "xptrace.h"
#include "statics.h"

namespace xptrace {

    std::map<string, module::id> modules_by_name;
    std::vector<module> modules;

    std::vector<marker> markers;

    bool logging_enabled = false;

}