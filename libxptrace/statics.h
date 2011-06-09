#include <map>
#include <vector>

namespace xptrace {

    extern std::map<string, module::id> modules_by_name;
    extern std::vector<module> modules;

    extern std::vector<marker> markers;

    extern bool logging_enabled;

}