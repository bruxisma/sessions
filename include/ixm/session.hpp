#ifndef IXM_SESSION_HPP
#define IXM_SESSION_HPP

#include <string>
#include <string_view>

namespace ixm::session {

struct environment
{
  class variable 
  {
    operator std::string_view () const noexcept;
    variable& operator = (std::string_view);
    std::string_view key () const noexcept;
    // /* implementation-defined */ split () const;
  };

  using value_range = void /* implementation-defined */;
  using key_range = void /* implementation-defined */;
  using iterator = void /* implementation-defined */;
  using value_type = variable;
  using size_type = size_t;

  template <class T>
  value_type operator [] (T const&) const;

  value_type operator [] (std::string const&) const noexcept;
  value_type operator [] (std::string_view) const;
  value_type operator [] (char const*) const noexcept;

  template <class K>
  iterator find (K const&) const noexcept;

  bool contains (std::string_view) const noexcept;

  iterator cbegin () const noexcept;
  iterator cend () const noexcept;

  iterator begin () const noexcept;
  iterator end () const noexcept;

  size_type size () const noexcept;
  bool empty () const noexcept;

  value_range values () const noexcept;
  key_range keys () const noexcept;

  template <class K>
  void erase (K const&) noexcept;
};

struct arguments
{
  class iterator
  {
  public:
    using value_type = const char*;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::random_access_iterator_tag;

    explicit iterator(size_t index = 0);

    iterator& operator ++ ();

    iterator& operator ++ (int) {
        auto tmp = iterator(*this);
        operator++();
        return tmp;
    }

    bool operator == (const iterator& rhs) const {
        return m_arg == rhs.m_arg;
    }
    
    bool operator != (const iterator& rhs) const {
        return m_arg != rhs.m_arg;
    }

    reference operator * () { return m_arg; }

  private:
      value_type m_arg = nullptr;
      size_t m_idx = 0;
  };

  using reverse_iterator = std::reverse_iterator<iterator>;
  using value_type = std::string_view;
  using index_type = size_t;
  using size_type = size_t;

  value_type operator [] (index_type) const noexcept;
  value_type at (index_type) const noexcept(false);

  [[nodiscard]] bool empty () const noexcept;
  size_type size () const noexcept;

  iterator cbegin () const noexcept;
  iterator cend () const noexcept;

  iterator begin () const noexcept { return cbegin(); }
  iterator end () const noexcept   { return cend(); }

  reverse_iterator crbegin () const noexcept;
  reverse_iterator crend () const noexcept;

  reverse_iterator rbegin () const noexcept { return crbegin(); }
  reverse_iterator rend () const noexcept { return crend(); }

  [[nodiscard]] const char** argv () const noexcept;
  [[nodiscard]] int argc () const noexcept;
};

} /* namespace ixm::session */

#endif /* IXM_SESSION_HPP */
