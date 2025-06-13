struct vector_3d { float x , y , z; };

bool world_to_screen( const vector_3d& vIn , vector_3d& vOut )
{
    FLOAT w = da_bois[ 3 ][ 0 ] * vIn.x + da_bois[ 3 ][ 1 ] * vIn.y + da_bois[ 3 ][ 2 ] * vIn.z + da_bois[ 3 ][ 3 ];

    if ( w < 0.01f )
        return false;

    vOut.x = da_bois[ 0 ][ 0 ] * vIn.x + da_bois[ 0 ][ 1 ] * vIn.y + da_bois[ 0 ][ 2 ] * vIn.z + da_bois[ 0 ][ 3 ];
    vOut.y = da_bois[ 1 ][ 0 ] * vIn.x + da_bois[ 1 ][ 1 ] * vIn.y + da_bois[ 1 ][ 2 ] * vIn.z + da_bois[ 1 ][ 3 ];
    FLOAT invw = 1.0f / w;

    vOut.x *= invw;
    vOut.y *= invw;

    int width = 1920;
    int height = 1080;

    float x = width / 2.0f;
    float y = height / 2.0f;

    x += 0.5f * vOut.x * width + 0.5f;
    y -= 0.5f * vOut.y * height + 0.5f;

    vOut.x = x;
    vOut.y = y;

    return true;
}