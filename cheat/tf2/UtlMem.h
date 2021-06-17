#pragma once

typedef intptr_t intp;
inline int UtlMemory_CalcNewAllocationCount( int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem )
{
	if( nGrowSize )
		nAllocationCount = ( ( 1 + ( ( nNewSize - 1 ) / nGrowSize ) ) * nGrowSize );
	else
	{
		if( !nAllocationCount )
			nAllocationCount = ( 31 + nBytesItem ) / nBytesItem;

		while( nAllocationCount < nNewSize )
			nAllocationCount *= 2;
	}

	return nAllocationCount;
}

template< class T, class I = int >
class CUtlMemory
{
public:
	T & operator[]( I i )
	{
		return m_pMemory[ i ];
	}

	explicit operator bool( ) const noexcept {
		return !!( m_pMemory );
	}

	T *Base( )
	{
		return m_pMemory;
	}

	int NumAllocated( ) const
	{
		return m_nAllocationCount;
	}

	void Grow( int num = 1 )
	{
		if( IsExternallyAllocated( ) )
			return;

		int nAllocationRequested = m_nAllocationCount + num;
		int nNewAllocationCount = UtlMemory_CalcNewAllocationCount( m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof( T ) );

		if( ( int )( I )nNewAllocationCount < nAllocationRequested )
		{
			if( ( int )( I )nNewAllocationCount == 0 && ( int )( I )( nNewAllocationCount - 1 ) >= nAllocationRequested )
				--nNewAllocationCount;
			else
			{
				if( ( int )( I )nAllocationRequested != nAllocationRequested )
					return;

				while( ( int )( I )nNewAllocationCount < nAllocationRequested )
					nNewAllocationCount = ( nNewAllocationCount + nAllocationRequested ) / 2;
			}
		}

		m_nAllocationCount = nNewAllocationCount;

		if( m_pMemory )
			m_pMemory = ( T * )realloc( m_pMemory, m_nAllocationCount * sizeof( T ) );
		else
			m_pMemory = ( T * )malloc( m_nAllocationCount * sizeof( T ) );
	}

	bool IsExternallyAllocated( ) const
	{
		return m_nGrowSize < 0;
	}

protected:
	T * m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template <class T>
inline T *Construct( T *pMemory )
{
	return ::new( pMemory ) T;
}

template <class T>
inline T *CopyConstruct( T *pMemory, T const& src )
{
	return( new( pMemory ) T( src ) );
}

template <class T>
inline void Destruct( T *pMemory )
{
	pMemory->~T( );
}

template< class T >
class CUtlFixedMemory
{
public:
	// constructor, destructor
	CUtlFixedMemory( int nGrowSize = 0, int nInitSize = 0 );
	~CUtlFixedMemory( );

	// Set the size by which the memory grows
	void Init( int nGrowSize = 0, int nInitSize = 0 );

	// here to match CUtlMemory, but only used by ResetDbgInfo, so it can just return NULL
	T* Base( ) { return NULL; }
	const T* Base( ) const { return NULL; }

protected:
	struct BlockHeader_t;

public:
	class Iterator_t
	{
	public:
		Iterator_t( BlockHeader_t *p, int i ) : m_pBlockHeader( p ), m_nIndex( i ) {}
		BlockHeader_t *m_pBlockHeader;
		intp m_nIndex;

		bool operator==( const Iterator_t it ) const { return m_pBlockHeader == it.m_pBlockHeader && m_nIndex == it.m_nIndex; }
		bool operator!=( const Iterator_t it ) const { return m_pBlockHeader != it.m_pBlockHeader || m_nIndex != it.m_nIndex; }
	};
	Iterator_t First( ) const { return m_pBlocks ? Iterator_t( m_pBlocks, 0 ) : InvalidIterator( ); }
	Iterator_t Next( const Iterator_t &it ) const
	{
		Assert( IsValidIterator( it ) );
		if( !IsValidIterator( it ) )
			return InvalidIterator( );

		BlockHeader_t *pHeader = it.m_pBlockHeader;
		if( it.m_nIndex + 1 < pHeader->m_nBlockSize )
			return Iterator_t( pHeader, it.m_nIndex + 1 );

		return pHeader->m_pNext ? Iterator_t( pHeader->m_pNext, 0 ) : InvalidIterator( );
	}
	intp GetIndex( const Iterator_t &it ) const
	{
		Assert( IsValidIterator( it ) );
		if( !IsValidIterator( it ) )
			return InvalidIndex( );

		return ( intp )( HeaderToBlock( it.m_pBlockHeader ) + it.m_nIndex );
	}
	bool IsIdxAfter( intp i, const Iterator_t &it ) const
	{
		Assert( IsValidIterator( it ) );
		if( !IsValidIterator( it ) )
			return false;

		if( IsInBlock( i, it.m_pBlockHeader ) )
			return i > GetIndex( it );

		for( BlockHeader_t *pbh = it.m_pBlockHeader->m_pNext; pbh; pbh = pbh->m_pNext )
		{
			if( IsInBlock( i, pbh ) )
				return true;
		}
		return false;
	}
	bool IsValidIterator( const Iterator_t &it ) const { return it.m_pBlockHeader && it.m_nIndex >= 0 && it.m_nIndex < it.m_pBlockHeader->m_nBlockSize; }
	Iterator_t InvalidIterator( ) const { return Iterator_t( NULL, INVALID_INDEX ); }

	// element access
	T& operator[]( intp i );
	const T& operator[]( intp i ) const;
	T& Element( intp i );
	const T& Element( intp i ) const;

	// Can we use this index?
	bool IsIdxValid( intp i ) const;

	// Specify the invalid ('null') index that we'll only return on failure
	static const intp INVALID_INDEX = 0; // For use with COMPILE_TIME_ASSERT
	static intp InvalidIndex( ) { return INVALID_INDEX; }

	// Size
	int NumAllocated( ) const;
	int Count( ) const { return NumAllocated( ); }

	// Grows memory by max(num,growsize), and returns the allocation index/ptr
	void Grow( int num = 1 );

	// Makes sure we've got at least this much memory
	void EnsureCapacity( int num );

	// Memory deallocation
	void Purge( );

protected:
	// Fast swap - WARNING: Swap invalidates all ptr-based indices!!!
	void Swap( CUtlFixedMemory< T > &mem );

	bool IsInBlock( intp i, BlockHeader_t *pBlockHeader ) const
	{
		T *p = ( T* )i;
		const T *p0 = HeaderToBlock( pBlockHeader );
		return p >= p0 && p < p0 + pBlockHeader->m_nBlockSize;
	}

	struct BlockHeader_t
	{
		BlockHeader_t *m_pNext;
		intp m_nBlockSize;
	};

	const T *HeaderToBlock( const BlockHeader_t *pHeader ) const { return ( T* )( pHeader + 1 ); }
	const BlockHeader_t *BlockToHeader( const T *pBlock ) const { return ( BlockHeader_t* )( pBlock )-1; }

	BlockHeader_t* m_pBlocks;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template< class T, class A = CUtlMemory<T> >
class CUtlVector
{
	typedef A CAllocator;

	typedef T *iterator;
	typedef const T *const_iterator;
public:
	T & operator[]( int i )
	{
		return m_Memory[ i ];
	}

	explicit operator bool( ) const noexcept {
		return !!( m_Memory );
	}

	T& Element( int i )
	{
		return m_Memory[ i ];
	}

	T* Base( )
	{
		return m_Memory.Base( );
	}

	int Count( ) const
	{
		return m_Size;
	}

	void RemoveAll( )
	{
		for( int i = m_Size; --i >= 0; )
			Destruct( &Element( i ) );

		m_Size = 0;
	}

	int AddToTail( const T& src )
	{
		return InsertBefore( m_Size, src );
	}

	int AddToTail( )
	{
		return InsertBefore( m_Size );
	}

	int InsertBefore( int elem, const T& src )
	{
		Growvec3_t( );
		ShiftElementsRight( elem );
		CopyConstruct( &Element( elem ), src );
		return elem;
	}

	int InsertBefore( int elem )
	{
		Growvec3_t( );
		ShiftElementsRight( elem );
		Construct( &Element( elem ) );

		return elem;
	}

	T* GetElements( void ){
		return m_pElements;
	}

	iterator begin( ) { return Base( ); }
	const_iterator begin( ) const { return Base( ); }
	iterator end( ) { return Base( ) + Count( ); }
	const_iterator end( ) const { return Base( ) + Count( ); }

protected:
	void Growvec3_t( int num = 1 )
	{
		if( m_Size + num > m_Memory.NumAllocated( ) )
			m_Memory.Grow( m_Size + num - m_Memory.NumAllocated( ) );

		m_Size += num;
		ResetDbgInfo( );
	}

	void ShiftElementsRight( int elem, int num = 1 )
	{
		int numToMove = m_Size - elem - num;
		if( ( numToMove > 0 ) && ( num > 0 ) )
			memmove( &Element( elem + num ), &Element( elem ), numToMove * sizeof( T ) );
	}

	CAllocator m_Memory;
	int m_Size;

	T *m_pElements;

	inline void ResetDbgInfo( )
	{
		m_pElements = Base( );
	}
};