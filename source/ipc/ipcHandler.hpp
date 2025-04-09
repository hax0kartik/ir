#include "../ir.hpp"
#include "irrstHandler.hpp"
#include "iruHandler.hpp"
#include "irUserHandler.hpp"

namespace ir {

template <Services service>
constexpr void Handle() {
    if constexpr (service == Services::IRRST) {
        ir::rst::Handle();
    } else if constexpr (service == Services::IRU) {
        ir::u::Handle();
    } else if constexpr (service == Services::IRUSER) {
        ir::user::Handle();
    }
}

} // End of namespace ir
