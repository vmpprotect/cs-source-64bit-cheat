#include <includes.h>

void draw_imgui( ImDrawList* drawList , int width , int height ) {
    da_bois = RPM<ViewMatrix>( engine_base + source::offsets::view_matrix );
    player* localPlayer = RPM<player*>( client_base + source::offsets::local_player );
    int localTeam = localPlayer->get_team( );
    vector_3d localPos = localPlayer->get_orgin( );

    ImGui::Text( "localPlayer | %llx" , localPlayer );
    ImGui::Text( "localTeam | %i" , localTeam );
    ImGui::Text( "localPos.x | %f" , localPos.x );
    ImGui::Text( "localPos.y | %f" , localPos.y );
    ImGui::Text( "localPos.z | %f" , localPos.z );
    ImGui::Separator( );
    ImGui::Text( "ViewMatrix:" );
    for ( int i = 0; i < 4; i++ ) {
        ImGui::Text( "| %.2f %.2f %.2f %.2f |" ,
            da_bois.matrix[ i * 4 + 0 ] ,
            da_bois.matrix[ i * 4 + 1 ] ,
            da_bois.matrix[ i * 4 + 2 ] ,
            da_bois.matrix[ i * 4 + 3 ] );
    }
    ImGui::Separator( );
    ImGui::Text( "overlay fps | %f" , ImGui::GetIO( ).Framerate );

    for ( int i = 0; i < 128; ++i ) {
        player* entity = RPM<player*>( client_base + source::offsets::entity_list + i * source::offsets::entity_size );
        if ( !entity ) continue;
        auto is_doormant = entity->get_dormant_state( );
        if ( is_doormant ) continue;
        int health = entity->get_health( );
        if ( health > 500 || health == 0 ) continue; // some random things show up and they have a random value of health health
        std::string health_text = std::to_string( health );
        int team = entity->get_team( );
        if ( team == localTeam ) continue;

        vector_3d enemyPos = entity->get_orgin( );
        vector_3d screen;
        world_to_screen( enemyPos , screen );
        drawList->AddLine( ImVec2( width / 2 , height / 2 ) , ImVec2( screen.x , screen.y ) , IM_COL32( 255 , 0 , 0 , 255 ) , 1.5f  );

        std::string team_name;
        if ( team == 1 ) team_name = "Spectator";
        if ( team == 2 ) team_name = "T";
        if ( team == 3 ) team_name = "CT";

        drawList->AddText( ImVec2( screen.x , screen.y ) , IM_COL32( 255 , 255 , 255 , 255 ) , team_name.c_str( ) );
        drawList->AddText( ImVec2( screen.x , screen.y + 10 ) , IM_COL32( 255 , 255 , 255 , 255 ) , health_text.c_str( ) );

        int bones[ ][ 2 ] = {
            { 0, 1 },   // PELVIS -> LEFT_HIP
            { 1, 2 },   // LEFT_HIP -> LEFT_KNEE
            { 2, 3 },   // LEFT_KNEE -> LEFT_ANKLE
            { 3, 4 },   // LEFT_ANKLE -> LEFT_FOOT

            { 0, 5 },   // PELVIS -> RIGHT_HIP
            { 5, 6 },   // RIGHT_HIP -> RIGHT_KNEE
            { 6, 7 },   // RIGHT_KNEE -> RIGHT_ANKLE
            { 7, 8 },   // RIGHT_ANKLE -> RIGHT_FOOT

            { 0, 9 },   // PELVIS -> STOMACH
            { 9, 10 },  // STOMACH -> SPINE
            { 10, 11 }, // SPINE -> CHEST
            { 11, 12 }, // CHEST -> NECK
            { 12, 14 }, // NECK -> HEAD
            { 14, 13 }, // HEAD -> FACE

            { 11, 16 }, // CHEST -> LEFT_SHOULDER
            { 16, 17 }, // LEFT_SHOULDER -> LEFT_ELBOW
            { 17, 18 }, // LEFT_ELBOW -> LEFT_HAND

            { 11, 29 }, // CHEST -> RIGHT_SHOULDER
            { 29, 30 }, // RIGHT_SHOULDER -> RIGHT_ELBOW
            { 30, 31 }  // RIGHT_ELBOW -> RIGHT_HAND
        };

        for ( auto& bone : bones ) {
            vector_3d bonePos1 = entity->get_bone_pos( bone[ 0 ] );
            vector_3d bonePos2 = entity->get_bone_pos( bone[ 1 ] );
            vector_3d screen1 , screen2;

            if ( world_to_screen( bonePos1 , screen1 ) &&
                world_to_screen( bonePos2 , screen2  ) ) {
                drawList->AddLine( 
                    ImVec2( screen1.x , screen1.y ) , 
                    ImVec2( screen2.x , screen2.y ) , 
                    IM_COL32( 0 , 255 , 0 , 255 )
                    , 1.0f
                );
            }
        }

    }
}

bool initalize_directx( HWND hwnd ) {
    pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if ( !pD3D ) return false;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferWidth = 1920;
    d3dpp.BackBufferHeight = 1080;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if ( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hwnd ,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , &d3d_device ) ) )
        return false;

    return true;
}

void create_overlay( ) {
    WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L,
        GetModuleHandle( 0 ), 0, 0, 0, 0, L"OverlayClass", 0 };
    RegisterClassEx( &wc );

    game_handle = FindWindowA( NULL , "Counter-Strike Source - Direct3D 9 - 64 Bit" );
    RECT game_dimensions;
    GetWindowRect( game_handle , &game_dimensions );

    overlay_handle = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE ,
        wc.lpszClassName , L"Overlay" , WS_POPUP ,
        game_dimensions.left , game_dimensions.top ,
        game_dimensions.right - game_dimensions.left , game_dimensions.bottom - game_dimensions.top ,
        NULL , NULL , wc.hInstance , NULL );

    SetLayeredWindowAttributes( overlay_handle , 0 , 255 , LWA_ALPHA );
    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea( overlay_handle , &margins );

    ShowWindow( overlay_handle , SW_SHOW );
    UpdateWindow( overlay_handle );

    LONG exStyle = GetWindowLong( overlay_handle , GWL_EXSTYLE );
    SetWindowLong( overlay_handle , GWL_EXSTYLE , exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT );
}

int APIENTRY WinMain( HINSTANCE , HINSTANCE , LPSTR , int ) {
	FILE* f;
    AllocConsole( );
	freopen_s( &f , "CONOUT$" , "w" , stdout );
    if ( !get_process( L"cstrike_win64.exe" , prcoess_id , process_handle ) ) {
        MessageBoxA( 0 , "CSS not running!" , "Error" , MB_OK );
        return 1;
    }

    client_base = get_module_base( L"client.dll" , prcoess_id );
    engine_base = get_module_base( L"engine.dll" , prcoess_id );
	printf( "Client.dll: 0x%p\nEngine.dll: 0x%p\n" , ( void* ) client_base , ( void* ) engine_base );
    create_overlay( );

    if ( !initalize_directx( overlay_handle ) ) return 1;

    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO& io = ImGui::GetIO( ); ( void ) io;

    ImGui_ImplWin32_Init( overlay_handle );
    ImGui_ImplDX9_Init( d3d_device );

    MSG msg;
    ZeroMemory( &msg , sizeof( msg ) );

    while ( msg.message != WM_QUIT ) {
        if ( PeekMessage( &msg , NULL , 0U , 0U , PM_REMOVE ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            continue;
        }

        RECT game_dimensions;
        GetWindowRect( game_handle , &game_dimensions );
        MoveWindow( overlay_handle , game_dimensions.left , game_dimensions.top , game_dimensions.right - game_dimensions.left , game_dimensions.bottom - game_dimensions.top , TRUE );
        int width = game_dimensions.right - game_dimensions.left;
        int height = game_dimensions.bottom - game_dimensions.top;

        d3d_device->Clear( 0 , NULL , D3DCLEAR_TARGET , 0 , 1.0f , 0 );
        if ( d3d_device->BeginScene( ) >= 0 ) {
            ImGui_ImplDX9_NewFrame( );
            ImGui_ImplWin32_NewFrame( );
            ImGui::NewFrame( );

            draw_imgui( ImGui::GetBackgroundDrawList( ) , width , height );

            ImGui::EndFrame( );
            ImGui::Render( );
            ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
            d3d_device->EndScene( );
        }
        d3d_device->Present( NULL , NULL , NULL , NULL );
    }

    ImGui_ImplDX9_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );
    d3d_device->Release( );
    pD3D->Release( );
    DestroyWindow( overlay_handle );
    return 0;
}
