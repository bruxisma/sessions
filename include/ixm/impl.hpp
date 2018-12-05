#ifndef SESSION_SRC_IMPL_HPP
#define SESSION_SRC_IMPL_HPP

#include <type_traits>
#include <cstddef>
#include <iterator>

namespace impl {

char const* argv (std::size_t) noexcept;
char const** argv() noexcept;
int argc () noexcept;

char const** envp () noexcept;
void set_env(const char*, const char*) noexcept;
extern const char env_path_sep;


class charbuff_iterator
{
public:
    using value_type = const char*;
    using reference = value_type&;
    using difference_type = ptrdiff_t;
    using pointer = value_type * ;
    using iterator_category = std::random_access_iterator_tag;

    explicit charbuff_iterator(const char** buff = nullptr, size_t index = 0) :
        m_buff(buff), m_idx(index)
    {
    }

    charbuff_iterator& operator ++ ()
    {
        ++m_buff;
        return *this;
    }

    charbuff_iterator& operator ++ (int) {
        auto tmp = charbuff_iterator(*this);
        operator++();
        return tmp;
    }

    bool operator == (const charbuff_iterator& rhs) const {
        return current() == rhs.current();
    }

    bool operator != (const charbuff_iterator& rhs) const {
        return current() != rhs.current();
    }

    reference operator * () { return current(); }

protected:
    reference current() const { return m_buff[m_idx]; }
    reference current() { return m_buff[m_idx]; }

private:
    const char** m_buff = nullptr;
    size_t m_idx = 0;
};

} /* namespace impl */

#endif /* SESSION_SRC_IMPL_HPP */
