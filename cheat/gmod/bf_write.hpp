#pragma once

class bf_write {
public:
	bf_write( void* data, int bytes, int max_bits = -1 ) {
		m_data = ( unsigned char* )data;
		m_bytes = bytes;
		if( max_bits == -1 )
			m_bits = bytes * 8;
		else
			m_bits = max_bits;
	}

	bf_write( const char* debug_name, void* data, int bytes, int max_bits = -1 ) {
		m_debug_name = debug_name;
		m_data = ( unsigned char* )data;
		m_bytes = bytes;
		if( max_bits == -1 )
			m_bits = bytes * 8;
		else
			m_bits = max_bits;
	}

public:
	unsigned char*	m_data{ };
	int				m_bytes{ };
	int				m_bits{ };
	int				m_cur_bit{ };

private:
	bool			m_overflow{ };
	bool			m_assert_on_overflow{ };
	const char*		m_debug_name{ };
};