struct ViewMatrix {
    float matrix[ 16 ];
    float* operator[]( int index ) {
        return &matrix[ index * 4 ];
    }
    const float* operator[]( int index ) const {
        return &matrix[ index * 4 ];
    }
}; ViewMatrix da_bois;

struct BoneMatrix {
    float matrix[ 3 ][ 4 ];
};
