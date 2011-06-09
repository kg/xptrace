#include <map>
#include <vector>

namespace xptrace {

    struct callback_entry {
        marker_callback callback;
        void * userdata;
    };

    struct marker {
        string name;
        const void * return_address;
        bool enabled, initialized;
        unsigned char original_bytes[SIZEOF_CALL];
        std::vector<callback_entry> callbacks;
    };

    extern std::vector<marker> markers;

    extern bool logging_enabled;

}