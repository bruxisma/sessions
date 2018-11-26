#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

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
  
  auto vec = std::vector<std::unique_ptr<char const[]>>();
  vec.reserve(argc);
  
  for(int i = 0; i < argc; i++)
  {
    vec.push_back(to_utf8(wargv[i]));
  }
  
  LocalFree(wargv);

  return vec;
}

auto const& vector () {
  static auto value = initialize();
  return value;
}

// _wenviron is still a depricated thing, but its way simpler to convert its
// contents to utf8 then to handle GetEnvironmentStrings manually
auto initialize_environ()
{
  // make sure _wenviron is initialized
  // https://docs.microsoft.com/en-us/cpp/c-runtime-library/environ-wenviron?view=vs-2017#remarks
  if (!_wenviron) {
    _wgetenv(L"initpls");
  }

  wchar_t** wide_environ = _wenviron;
  size_t var_count = 0;
  while (wide_environ[var_count++]);

  auto env = std::make_unique<const char*[]>(var_count);
  auto** result_it = env.get();

  for(auto current = *wide_environ; current; current = *++wide_environ, result_it++)
  {
    auto converted_var = to_utf8(current);
    *result_it = converted_var.release();
  }

  return env;
}

} /* nameless namespace */

namespace impl {

char const* argv (std::size_t idx) noexcept {
  return ::vector()[idx].get();
}

int argc () noexcept { return static_cast<int>(vector().size()); }

char const** envp () noexcept {
  static auto env = initialize_environ();
  return env.get();
}

} /* namespace impl */
