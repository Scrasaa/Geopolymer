#pragma once

#include "../../pch.h"
#include "ntdll.h"

class CMemory
{
private:
    DWORD GetHashFromString(char* szString) const;

    LPCWSTR ConvertToLPCWSTR(const char* szModuleName) const;

    int CmpUnicodeStr(const WCHAR* substr, const WCHAR* mystr);

protected:
    PEB* GetPEB() const;
public:

    HMODULE ResolveModuleBaseAddressPEB(char* szModuleName) const;

    PDWORD GetFunctionAddressByHash(char* library, DWORD hash) const;

    void* GetFunctionAddress(char* MyNtdllFunction, PVOID MyDLLBaseAddress) const;

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

class CPatternScan : protected CMemory
{
private:
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

