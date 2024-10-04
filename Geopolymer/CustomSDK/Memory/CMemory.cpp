#include "CMemory.h"

#pragma warning ( disable : 6387 )
#pragma warning ( disable : 4244 )

CPatternScan* g_PatternScan = nullptr;
CMemory* g_Memory = nullptr;

DWORD CMemory::GetHashFromString(char* szString) const
{
    size_t iLength = strnlen_s(szString, 50);
    DWORD hash = 0x35;

    for (size_t i = 0; i < iLength; i++)
    {
        hash += (hash * 0xab10f29f + szString[i]) & 0xffffff;
    }

    return hash;
}

// Structure with ProcessInformation
PEB* CPatternScan::GetPEB()
{
#ifdef _WIN64
    auto peb = (PEB*)__readgsword(0x60);

#else
    auto peb = (PEB*)__readfsdword(0x30);
#endif

    return peb;
}

HMODULE CMemory::ResolveModuleBaseAddressPEB(char* szModuleName) const
{
    // Convert the input module name to LPCWSTR
    LPCWSTR lModuleName = ConvertToLPCWSTR(szModuleName);
    if (!lModuleName)
        return nullptr;
    // move getpeb to cmemory and use it instead
    // Access the PEB structure
    auto pPeb = (PEB*)__readfsdword(0x30);

    PEB_LDR_DATA* pLdr = pPeb->Ldr;
    LIST_ENTRY* pListHead = &pLdr->InMemoryOrderModuleList;
    LIST_ENTRY* pCurrent = pListHead->Flink;

    // Iterate through the loaded modules
    while (pCurrent != pListHead) {
        // Get the current LDR_DATA_TABLE_ENTRY
        auto pEntry = CONTAINING_RECORD(pCurrent, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        // Compare the module names safely
        if (pEntry->BaseDllName.Buffer != NULL) {
            if (_wcsicmp(pEntry->BaseDllName.Buffer, lModuleName) == 0) {
                // Found the DLL, return its base address
                delete[] lModuleName; // Clean up
                return (HMODULE)pEntry->DllBase; // Return the base address
            }
        }

        // Move to the next entry
        pCurrent = pCurrent->Flink;
    }

    // Clean up if not found
    delete[] lModuleName; // Clean up
    return nullptr;
}

PDWORD CMemory::GetFunctionAddressByHash(char* library, DWORD hash)
{
    PDWORD functionAddress = nullptr;

    // Get base address of the module in which our exported function of interest resides (kernel32 in the case of CreateThread)
    HMODULE libraryBase = ResolveModuleBaseAddressPEB(library);

    auto dosHeader = (PIMAGE_DOS_HEADER)libraryBase;
    auto imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD)libraryBase + dosHeader->e_lfanew);

    DWORD exportDirectoryRVA = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

    auto imageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)libraryBase + exportDirectoryRVA);

    // Get RVAs to exported function related information
    auto addresOfFunctionsRVA = (PDWORD)((DWORD)libraryBase + imageExportDirectory->AddressOfFunctions);
    auto addressOfNamesRVA = (PDWORD)((DWORD)libraryBase + imageExportDirectory->AddressOfNames);
    auto addressOfNameOrdinalsRVA = (PWORD)((DWORD)libraryBase + imageExportDirectory->AddressOfNameOrdinals);

    // Iterate through exported functions, calculate their hashes and check if any of them match our hash of 0x00544e304 (CreateThread)
    // If yes, get its virtual memory address (this is where CreateThread function resides in memory of our process)
    for (DWORD i = 0; i < imageExportDirectory->NumberOfFunctions; i++)
    {
        DWORD functionNameRVA = addressOfNamesRVA[i];
        DWORD functionNameVA = (DWORD)libraryBase + functionNameRVA;
        auto* functionName = (char*)functionNameVA;
        DWORD functionAddressRVA = 0;

        // Calculate hash for this exported function
        DWORD functionNameHash = GetHashFromString(functionName);

        // If hash for CreateThread is found, resolve the function address
        if (functionNameHash == hash)
        {
            functionAddressRVA = addresOfFunctionsRVA[addressOfNameOrdinalsRVA[i]];
            functionAddress = (PDWORD)((DWORD)libraryBase + functionAddressRVA);
            //printf("%s : 0x%x : %p\n", functionName, functionNameHash, functionAddress);
            return functionAddress;
        }
    }
}

void* CMemory::GetFunctionAddress(char* MyNtdllFunction, PVOID MyDLLBaseAddress) const
{
    DWORD j;
    uintptr_t RVA = 0;

    //Parse DLL loaded in memory
    const auto BaseDLLAddr = (LPVOID)MyDLLBaseAddress;
    auto pImgDOSHead = (PIMAGE_DOS_HEADER)BaseDLLAddr;
    auto pImgNTHead = (PIMAGE_NT_HEADERS)((DWORD_PTR)BaseDLLAddr + pImgDOSHead->e_lfanew);

    //Get the Export Directory Structure
    auto pImgExpDir = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)BaseDLLAddr + pImgNTHead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    //Get the functions RVA array
    auto Address = (PDWORD)((LPBYTE)BaseDLLAddr + pImgExpDir->AddressOfFunctions);

    //Get the function names array 
    auto Name = (PDWORD)((LPBYTE)BaseDLLAddr + pImgExpDir->AddressOfNames);

    //get the Ordinal array
    auto Ordinal = (PWORD)((LPBYTE)BaseDLLAddr + pImgExpDir->AddressOfNameOrdinals);

    //Get RVA of the function from the export table
    for (j = 0; j < pImgExpDir->NumberOfNames; j++) {
        if (!strcmp(MyNtdllFunction, (char*)BaseDLLAddr + Name[j])) {
            //if function name found, we retrieve the RVA
            RVA = (uintptr_t)((LPBYTE)Address[Ordinal[j]]);
            break;
        }
    }

    if (RVA) {
        //Compute RVA to find the current address in the process
        uintptr_t moduleBase = (uintptr_t)BaseDLLAddr;
        uintptr_t* TrueAddress = (uintptr_t*)(moduleBase + RVA);
        return (PVOID)TrueAddress;
    }
    else {
        return (PVOID)RVA;
    }
}

LPCWSTR CMemory::ConvertToLPCWSTR(const char* szModuleName) const
{
    if (!szModuleName)
        return nullptr;

    // Calculate the required size for the wide string
    int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, szModuleName, -1, nullptr, 0);
    if (wideCharSize <= 0)
        return nullptr; // Handle error

    // Allocate memory for the wide string
    wchar_t* wideString = new wchar_t[wideCharSize];

    // Perform the conversion
    MultiByteToWideChar(CP_UTF8, 0, szModuleName, -1, wideString, wideCharSize);

    return wideString; // Return the converted string, need to handle memory deallocation
}

int CMemory::CmpUnicodeStr(const WCHAR* substr, const WCHAR* mystr)
{
    // Convert both strings to lowercase using _wcslwr_s
    WCHAR lowerSubstr[MAX_PATH];
    WCHAR lowerMystr[MAX_PATH];

    // Copy and lowercase the input strings
    wcscpy_s(lowerSubstr, MAX_PATH, substr);
    wcscpy_s(lowerMystr, MAX_PATH, mystr);
    _wcslwr_s(lowerSubstr, MAX_PATH);
    _wcslwr_s(lowerMystr, MAX_PATH);

    // Check if the substring exists in the main string
    return (wcsstr(lowerMystr, lowerSubstr) != NULL) ? 1 : 0;
}

void CPatternScan::Parse(char* combo, char* pattern, char* mask)
{
    char lastChar = ' ';
    unsigned int j = 0;

    for (unsigned int i = 0; i < strlen(combo); i++)
    {
        if ((combo[i] == '?' || combo[i] == '*') && (lastChar != '?' && lastChar != '*'))
        {
            pattern[j] = mask[j] = '?';
            j++;
        }

        else if (isspace(lastChar))
        {
            pattern[j] = lastChar = (char)strtol(&combo[i], 0, 16);
            mask[j] = 'x';
            j++;
        }
        lastChar = combo[i];
    }
    pattern[j] = mask[j] = '\0';
}

char* CPatternScan::PatternScan(char* pattern, char* mask, char* begin, size_t size)
{
    size_t iPatternLength = strlen(mask);

    // Loop through memory region
    for (int i = 0; i < size; i++)
    {
        bool bFound = true;
        // Loop to check pattern and mask
        for (int j = 0; j < iPatternLength; j++)
        {
            // Check if it is not a wildcard and not matching pattern
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
            {
                bFound = false;
                break;
            }
        }

        if (bFound)
            return begin + i;

    }
    return nullptr;
}

char* CPatternScan::TO_CHAR(wchar_t* string)
{
    size_t len = wcslen(string) + 1;
    char* c_string = new char[len];
    size_t numCharsRead;
    // Converts a sequence of wide characters to a corresponding sequence of multibyte characters
    wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
    return c_string;
}
LDR_DATA_TABLE_ENTRY* CPatternScan::GetLDREntry(std::string name)
{
    // Contains information about the loaded modules for the process
    LDR_DATA_TABLE_ENTRY* ldr = nullptr;

    auto peb = GetPEB();

    // Structure for a doubly linked list
    LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList; // inMemoryOrderModuleList = head of a doubly linked list that contains loaded moduels in the process!

    // Buffer of the linked list head
    LIST_ENTRY curr = head;

    // While begin != end, linked list (goes forward until last list is reached) -> just interates trhough MemoryOrderModuleList
    while (curr.Flink != head.Blink)
    {
        // Base Adress of the module struc
        LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (mod->FullDllName.Buffer)
        {
            // Get module names
            char* cName = TO_CHAR(mod->BaseDllName.Buffer);

            // Comapre moduel names with our input module name
            if (_stricmp(cName, name.c_str()) == 0)
            {
                ldr = mod;
                break;
            }
            delete[] cName;
        }
        curr = *curr.Flink; // curr = next list
    }
    // Returns the structure of the found module
    return ldr;
}

intptr_t CPatternScan::InPatternScan(char* combopattern, std::string szModName)
{
    // Getting the struc module informations of our module
    LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(szModName);

    char pattern[100];
    char mask[100];
    Parse(combopattern, pattern, mask);

    char* match = ScanInWrapper(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

    return (intptr_t)match;
}

intptr_t CPatternScan::InPatternScan(char* szPattern, char* szMask, std::string szModName)
{
    // Getting the struc module informations of our module
    LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(szModName);

    char* match = ScanInWrapper(szPattern, szMask, (char*)ldr->DllBase, ldr->SizeOfImage);

    return (intptr_t)match;
}

char* CPatternScan::ScanInWrapper(char* pattern, char* mask, char* begin, size_t size)
{
    char* match{ nullptr };

    // Contains information about a range of pages in the virtual address space of a process. The VirtualQuery and VirtualQueryEx functions use this structure.
    MEMORY_BASIC_INFORMATION mbi{};

    // Iterates through memory regionsW
    for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
    {
        // Checks if the memory regions are accessable and readable, if not we skip
        if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

        // If we found a valid memory region, we start our scan
        match = PatternScan(pattern, mask, curr, mbi.RegionSize);

        if (match != nullptr)
            break;
    }
    return match;
}