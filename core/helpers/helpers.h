LRESULT CALLBACK WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam ) {
    if ( ImGui_ImplWin32_WndProcHandler( hWnd , msg , wParam , lParam ) ) return true;
    if ( msg == WM_DESTROY ) PostQuitMessage( 0 );
    return DefWindowProc( hWnd , msg , wParam , lParam );
}

bool get_process( const wchar_t* name , DWORD& pid , HANDLE& hProc ) {
    PROCESSENTRY32 entry = { sizeof( entry ) };
    HANDLE snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , 0 );
    while ( Process32Next( snap , &entry ) ) {
        if ( !_wcsicmp( entry.szExeFile , name ) ) {
            pid = entry.th32ProcessID;
            hProc = OpenProcess( PROCESS_ALL_ACCESS , FALSE , pid );
            CloseHandle( snap );
            return true;
        }
    }
    CloseHandle( snap );
    return false;
}

uintptr_t get_module_base( const wchar_t* modName , DWORD pid ) {
    MODULEENTRY32 mod = { sizeof( mod ) };
    HANDLE snap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 , pid );
    while ( Module32Next( snap , &mod ) ) {
        if ( !_wcsicmp( mod.szModule , modName ) ) {
            CloseHandle( snap );
            return ( uintptr_t ) mod.modBaseAddr;
        }
    }
    CloseHandle( snap );
    return 0;
}

template<typename T>
T RPM( uintptr_t address ) {
    T buffer;
    ReadProcessMemory( process_handle , ( LPCVOID ) address , &buffer , sizeof( T ) , nullptr ); // hell yeah.
    return buffer;
}