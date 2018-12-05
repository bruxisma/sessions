#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <shellapi.h>

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

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

auto initialize_args () {
  auto cl = GetCommandLineW();
  int argc; // skip invoke command
  auto wargv = CommandLineToArgvW(cl, &argc)+1;

  auto vec = std::vector<char const*>(argc, nullptr);

  for(int i = 0; i < argc-1; i++)
  {
    vec[i] = to_utf8(wargv[i]).release();
  }
  
  LocalFree(wargv);

  return vec;
}

auto& args_vector () {
  static auto value = initialize_args();
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
  while (wide_environ[var_count])
    var_count++;

  auto vec = std::vector<char const*>(var_count+1, nullptr);

  for (size_t i = 0; i < var_count; i++)
  {
    vec[i] = to_utf8(wide_environ[i]).release();
  }

  return vec;
}

} /* nameless namespace */

namespace impl {

char const* argv (std::size_t idx) noexcept {
  return ::args_vector()[idx];
}

char const** argv() noexcept { 
    return ::args_vector().data();
}

int argc () noexcept { return static_cast<int>(args_vector().size()); }

char const** envp () noexcept {
  static auto env = initialize_environ();
  return env.data();
}

const char env_path_sep = ';';

} /* namespace impl */
