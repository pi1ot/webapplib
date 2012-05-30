function unsigned( i ) {
	if ( i < 0 ) {
		return i + 4294967296;
	} else if ( i >= 4294967296 ) {
		return i - 4294967296;
	} else {
		return i;
	}
}
function string_hash( str ) {
	var ch;
	var hash = 5381;
	var len = str.length;
	for ( var i=0; i<len; i++ ) {
		ch = str.charCodeAt(i) - 65; // 65:'A'
		if ( ch < 0 ) {
			ch += 256;
		}
		if ( ch <= 25 ) { // 25:'Z'-'A'
			ch += 32; // 32:'a'-'A'
		}
		hash = hash << 5;
		hash = unsigned( hash );
		hash += hash;
		hash = unsigned( hash );
		hash = hash ^ ch;
		hash = unsigned( hash );
	}
	return hash;
}