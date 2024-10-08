#pragma once

#include "../../pch.h"
#include "ntdll.h"

class CMemory
{
private:
    DWORD GetHashFromString(char* szString);

    LPCWSTR ConvertToLPCWSTR(const char* szModuleName);

    int CmpUnicodeStr(const WCHAR* substr, const WCHAR* mystr);

public:

    HMODULE ResolveModuleBaseAddressPEB(char* szModuleName);

    PDWORD GetFunctionAddressByHash(char* library, DWORD hash);

    void* GetFunctionAddress(char* MyNtdllFunction, PVOID MyDLLBaseAddress);

    template<typename T>
    T GetVTableFunction(void* pBase, int iIndex)
    {
        uintptr_t const* pVTable = *reinterpret_cast<uintptr_t**>(pBase);
        uintptr_t addy = pVTable[iIndex];

        return reinterpret_cast<T>(addy);
    }

    static inline uintptr_t GetVirtual(void*** c, int idx)
    {
        return reinterpret_cast<uintptr_t>((*c)[idx]);
    }


};

class CPatternScan 
{
private:

    PEB* GetPEB();
    char* ScanInWrapper(char* pattern, char* mask, char* begin, size_t size);

    void Parse(char* combo, char* pattern, char* mask);

    char* PatternScan(char* pattern, char* mask, char* begin, size_t size);

    char* TO_CHAR(wchar_t* string);

    LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name);

public:
    intptr_t InPatternScan(char* combopattern, std::string szModName);
    intptr_t InPatternScan(char* szPattern, char* szMask, std::string szModName);
};



extern CMemory* g_Memory;
extern CPatternScan* g_PatternScan;

