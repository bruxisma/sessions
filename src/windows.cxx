#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <shellapi.h>

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <system_error>

#include <cstdlib>

#include "ixm/impl.hpp"


namespace {

    [[noreturn]]
    void throw_error(DWORD error = GetLastError())
    {
        throw std::system_error(std::error_code{ static_cast<int>(error), std::system_category() });
    }

    auto narrow(wchar_t const* wstr, char* ptr = nullptr, int length = 0) {
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

    auto wide(const char* nstr, wchar_t* ptr = nullptr, int lenght = 0) {
        return MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            nstr,
            -1,
            ptr,
            lenght
        );
    }

    auto to_utf8(wchar_t const* wstr) {
        auto length = narrow(wstr);
        auto ptr = std::make_unique<char[]>(length);
        auto result = narrow(wstr, ptr.get(), length);

        if (result == 0)
            throw_error();

        return ptr;
    }

    auto to_utf16(const char* nstr) {
        auto length = wide(nstr);
        auto ptr = std::make_unique<wchar_t[]>(length);
        auto result = wide(nstr, ptr.get(), length);
        
        if (result == 0)
            throw_error();

        return ptr;
    }

    auto initialize_args() {
        auto cl = GetCommandLineW();
        int argc; // skip invoke command
        auto wargv = CommandLineToArgvW(cl, &argc) + 1;

        auto vec = std::vector<char const*>(argc, nullptr);

        for (int i = 0; i < argc - 1; i++)
        {
            vec[i] = to_utf8(wargv[i]).release();
        }

        LocalFree(wargv);

        return vec;
    }

    auto& args_vector() {
        static auto value = initialize_args();
        return value;
    }


    std::unique_ptr<const char*[]> p_environ;
    std::vector<std::unique_ptr<char[]>> env_vector;
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
        while (wide_environ[var_count]) var_count++;

        // TODO: this sucks D:
        // vec handles lifetime
        auto vec = std::vector<std::unique_ptr<char[]>>();
        vec.reserve(var_count + 1);
        auto env = std::make_unique<const char*[]>(var_count + 1);

        for (size_t i = 0; i < var_count; i++)
        {
            auto elem = to_utf8(wide_environ[i]);

            env[i] = elem.get();
            vec.push_back(std::move(elem));
        }

        vec.emplace_back();

        env_vector.swap(vec);
        p_environ.swap(env);
    }


} /* nameless namespace */

namespace impl {

    char const* argv(std::size_t idx) noexcept {
        return ::args_vector()[idx];
    }

    char const** argv() noexcept {
        return ::args_vector().data();
    }

    int argc() noexcept { return static_cast<int>(args_vector().size()); }

    char const** envp() noexcept {
        initialize_environ();
        return p_environ.get();
    }

    void set_env_var(const char* key, const char* value) noexcept
    {
        //auto wkey   = to_utf16(key);
        //auto wvalue = to_utf16(value);
        //auto ec = _wputenv_s(wkey.get(), wvalue.get());

        auto ec = _putenv_s(key, value);
        _ASSERTE(ec == 0);
    }

    const char env_path_sep = ';';

} /* namespace impl */
