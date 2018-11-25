#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

#undef GetEnvironmentStrings

#include <vector>
#include <memory>
#include <string>

#include <cstdlib>

#include "impl.hpp"

namespace {

auto narrow (wchar_t const* wstr, char* ptr=nullptr, int length=0) {
  return WideCharToMultiByte(
      CP_UTF8,
      WC_ERR_INVALID_CHARS,
      wstr,
      -1,
      ptr,
      length,
      nullptr,
      nullptr);
}

auto to_utf8 (wchar_t const* wstr) noexcept {
  // This entire function is a mess, but at least we can do a damn
  // conversion. Still though, it's *yikes* all around.
  // XXX: Need to do some basic error handling...
  auto length = narrow(wstr);
  auto ptr = std::make_unique<char[]>(length);
  auto result = narrow(wstr, ptr.get(), length);
  return ptr;
}

std::vector<std::unique_ptr<char const[]>> initialize () {
  auto cl = GetCommandLineW();
  int argc;
  auto wargv = CommandLineToArgvW(cl, &argc);
  
  // TODO: Finish implementing this


  LocalFree(wargv);
}

auto const& vector () {
  static auto value = initialize();
  return value;
}


// Return the pointer one-past-the-end of the double null terminator
wchar_t const* find_end_of_double_null_terminated_sequence(wchar_t const* const first) noexcept
{
    wchar_t const* last = first;
    for (; *last != '\0'; last += wcslen(last) + 1);

    return last + 1; 
}

template<typename CharT>
size_t count_env_block_variables(CharT* const env_block) noexcept
{
  using traits = std::char_traits<CharT>;

  size_t count = 0;

  CharT* it = env_block;
  while(*it != '\0')
  {
    // ignore drive letter settings
    if (*it != '=')
      count++;

    // advances the iterator to the next string
    it += traits::length(it) + 1;
  }
  
  return count;
}


struct env_block_deleter
{
  void operator () (char* env_block) const noexcept {
    FreeEnvironmentStringsA(env_block);
  }

  void operator () (wchar_t* env_block) const noexcept {
    FreeEnvironmentStringsW(env_block);
  }
};

template<typename CharT>
using win_env_block_ptr = std::unique_ptr<CharT[], env_block_deleter>;

using os_env_block_ptr = std::unique_ptr<wchar_t[], env_block_deleter>;
using environ_ptr = std::unique_ptr<char*[]>;


// grab the os env and copy the strings into CRT allocated memory
std::unique_ptr<wchar_t[]> get_wide_os_env_block()
{
  os_env_block_ptr we { GetEnvironmentStringsW() };

  auto first = we.get();
  auto last = find_end_of_double_null_terminated_sequence(first);

  const size_t distance = last - first;

  auto winenv = std::make_unique<wchar_t[]>(distance);
  // TODO: error handling

  memcpy(winenv.get(), first, distance * sizeof(wchar_t));

  return winenv;
}

std::unique_ptr<char[]> get_utf8_os_env_block()
{
  os_env_block_ptr we { GetEnvironmentStringsW() };

  auto first = we.get();
  auto last = find_end_of_double_null_terminated_sequence(first);

  const size_t narrow_sz = narrow(first);
  
  if (narrow_sz == 0) {
    return nullptr;
  }
  
  auto winenv = std::make_unique<char[]>(narrow_sz);

  const int conversion_result = narrow(first, winenv.get(), narrow_sz);
  
  if (conversion_result == 0) {
    return nullptr;
  }

  return winenv;
}


// Creates a new environment
environ_ptr create_env(char* const env_block)
{
  auto var_count = count_env_block_variables(env_block);
  
  auto env = std::make_unique<char*[]>(var_count + 1);

  if (!env) return nullptr;

  auto* src_it = env_block;
  auto** result_it = env.get();
  
  while(*src_it != '\0')
  {
    const size_t required_count = strlen(src_it) + 1;

    if (*src_it != '=')
    {
      auto variable = std::make_unique<char[]>(required_count);

      if (!variable) return nullptr;

      strcpy_s(variable.get(), required_count, src_it);

      *result_it++ = variable.release();

      // ?? *result_it++ = src_it ??
    }

    // advances the iterator to the next string
    src_it += required_count;
  }
    
  return env;
}


char** p_environ = nullptr;

bool init_env()
{
  if (p_environ)
  {
    return true;
  }

  auto eblock = get_utf8_os_env_block();
  if (!eblock) return false;

  auto env = create_env(eblock.get());

  if (!env) return false;

  // ...
}


// initializes p_environ
void env_init()
{
  // init only once
  if (p_environ) return;

  wchar_t** wide_environ = _wenviron;
  size_t var_count = 0;
  while (*wide_environ++) var_count++;

  auto env = std::make_unique<char*[]>(var_count);
  auto** result_it = env.get();

  for(auto current = *wide_environ; current; current = *++wide_environ, result_it++)
  {
    auto converted_var = to_utf8(current);
    *result_it = converted_var.release();
  }

  p_environ = env.release();
}

} /* nameless namespace */

namespace impl {

char const* argv (std::size_t idx) noexcept {
  return ::vector()[idx].get();
}

int argc () noexcept { return static_cast<int>(vector().size()); }


char const** envp () noexcept {
  env_init();
  return p_environ;
}

} /* namespace impl */
