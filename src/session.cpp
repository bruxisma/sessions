#include "impl.hpp"
#include "ixm/session.hpp"


namespace ixm::session 
{
    // env



    arguments::iterator::iterator(size_t index) : m_idx(index)
    {
        m_arg = impl::argv(m_idx);
    }

    auto arguments::iterator::operator++() -> iterator&
    {
        m_arg = impl::argv(++m_idx);
        return *this;
    }
   

    // args
    arguments::value_type arguments::operator [] (arguments::index_type idx) const noexcept
    {
        return impl::argv(idx);
    }

    arguments::value_type arguments::at(arguments::index_type idx) const
    {
        if (idx >= size()) {
            throw std::out_of_range("invalid arguments subscript");
        }

        return impl::argv(idx);
    }

    bool arguments::empty() const noexcept
    {
        return size() == 0;
    }
    
    arguments::size_type arguments::size() const noexcept
    {
        return static_cast<size_type>(argc() - 1);
    }

    arguments::iterator arguments::cbegin () const noexcept
    {
        return iterator{};
    }

    arguments::iterator arguments::cend () const noexcept
    {
        return iterator(size());
    }


    arguments::reverse_iterator arguments::crbegin () const noexcept
    {
        return reverse_iterator{ cend() };
    }

    arguments::reverse_iterator arguments::crend () const noexcept
    {
        return reverse_iterator{ cbegin() };
    }


    const char** arguments::argv() const noexcept
    {
        return impl::argv();
    }

    int arguments::argc() const noexcept
    {
        return impl::argc();
    }
}