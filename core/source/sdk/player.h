class player {
public:
	std::uintptr_t get_address( ){
		return ( std::uintptr_t ) ( this );
	}

	int get_team( ) {
		return RPM<int>( get_address( ) + source::offsets::team );
	}

	int get_health( ) {
		return RPM<int>( get_address( ) + source::offsets::health );
	}

	vector_3d get_orgin( ) {
		return RPM<vector_3d>( get_address( ) + source::offsets::vector_orgin );
	}

	bool get_dormant_state( ) {
		return RPM<bool>( get_address( ) + source::offsets::is_dormant );
	}

	vector_3d get_bone_pos( int boneID ) {
		uintptr_t bone_base = RPM<uintptr_t>( get_address( ) + 0x810 );
		BoneMatrix bone = RPM<BoneMatrix>( bone_base + boneID * sizeof( BoneMatrix ) );
		return { bone.matrix[ 0 ][ 3 ], bone.matrix[ 1 ][ 3 ], bone.matrix[ 2 ][ 3 ] };
	}

}; std::unique_ptr<player>( c_player );