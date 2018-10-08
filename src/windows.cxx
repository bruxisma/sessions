#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <memory>

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

} /* nameless namespace */

namespace impl {

char const* argv (std::size_t idx) noexcept {
  return ::vector()[idx].get();
}

int argc () noexcept { return static_cast<int>(vector().size()); }

// TODO: This actually needs some work since it is not utf-8 safe and
// technically deprecated... :/
char const** envp () noexcept { return _environ; }

} /* namespace impl */
