#ifndef SESSION_SRC_IMPL_HPP
#define SESSION_SRC_IMPL_HPP

#include <type_traits>
#include <cstddef>

namespace impl {

char const* argv (std::size_t) noexcept;
int argc () noexcept;

char const** envp () noexcept;

} /* namespace impl */

#endif /* SESSION_SRC_IMPL_HPP */
