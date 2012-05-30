sub string_hash() {
	my ( $string ) = @_;
	my $ch = 0;
	my $char = '';
	my $h = 5381;
	my $len = length( $string );	
	for( $i=0; $i<$len; $i++ ) {
		$char = substr( $string, $i, 1 );
		$ch = ord( $char ) - 65;
		if( $ch < 0 ) {
			$ch = 256 + $ch;
		}
		if( $ch <= 26 ) {
			$ch += 32;
		}
		$h = ( $h << 5 ) + $h;
		
		while( $h >= 4294967296 ) {
			$h -= 4294967296;
		}
		$h = $h ^ $ch;
	}
	$h = sprintf( "%lu", $h );
}