#include "impl.hpp"
#include "ixm/session.hpp"
#include <cstdlib>


namespace ixm::session 
{
    // env
    environment::variable::operator std::string_view() const noexcept
    {
        return m_value;
    }

    auto environment::variable::operator=(std::string_view str) -> variable&
    {
        m_value = str;
        impl::set_env(m_key.data(), m_value.data());
        return *this;
    }

    std::string_view environment::variable::key() const noexcept
    {
        return m_key;
    }

    environment::variable::variable(const char* ptr)
    {
        m_key = ptr;
        
        auto eqpos = m_key.find('=');
        if (eqpos == std::string_view::npos)
        {
            m_key = {};
            return;
        }

        m_value = m_key.substr(eqpos);
        m_key.remove_suffix(m_key.length() - eqpos);
    }


    environment::iterator::iterator(size_t idx)
    {
        m_envp = impl::envp();
        m_arg = *(m_envp + idx);
    }

    auto environment::iterator::operator++() -> iterator&
    {
        m_arg = *++m_envp;
        return *this;
    }


    // args
    //arguments::iterator::iterator(size_t index) : m_idx(index)
    //{
    //    m_arg = impl::argv(m_idx);
    //}

    //auto arguments::iterator::operator++() -> iterator&
    //{
    //    m_arg = impl::argv(++m_idx);
    //    return *this;
    //}




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

    arguments::iterator arguments::begin() const noexcept
    {
        return cbegin();
    }

    arguments::iterator arguments::end() const noexcept
    {
        return cend();
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