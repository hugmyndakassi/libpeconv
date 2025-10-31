#include "peconv/function_resolver.h"

#include <iostream>
#include <cctype>

namespace util {
    std::string toLowercase(std::string str)
    {
        for (char& ch : str) {
            ch = std::tolower(static_cast<unsigned char>(ch));
        }
        return str;
    }
}; //namespace util

HMODULE peconv::default_func_resolver::load_library(LPCSTR lib_name)
{
    if (!lib_name) {
        return nullptr;
    }
    const std::string modName = util::toLowercase(lib_name);
    auto found = this->nameToModule.find(modName);
    if (found != this->nameToModule.end()) {
        return found->second;
    }
    const HMODULE mod = LoadLibraryA(lib_name);
    if (mod) {
#ifdef _DEBUG
        std::cout << "Loaded the DLL: " << lib_name << " : " << std::hex << mod << std::endl;
#endif //_DEBUG
        this->nameToModule[modName] = mod;
    }
    return mod;
}

FARPROC peconv::default_func_resolver::resolve_func(LPCSTR lib_name, LPCSTR func_name)
{
    HMODULE libBasePtr = load_library(lib_name);
    if (libBasePtr == NULL) {
        std::cerr << "Could not load the library: " << lib_name << std::endl;
        return NULL;
    }
    FARPROC hProc = GetProcAddress(libBasePtr, func_name);
    if (hProc == NULL) {
        ULONGLONG func_val = (ULONGLONG)func_name;
        //is only the first WORD filled?
        bool is_ord = (func_val & (0x0FFFF)) == func_val;
        std::cerr << "Could not load the function: " << lib_name << ".";
        if (is_ord) {
            std::cerr << std::hex << "0x" << func_val;
        }
        else {
            std::cerr << func_name;
        }
        std::cerr << std::endl;
        return NULL;
    }
    return hProc;
}
