#pragma once

template <typename T, typename I = int>
class CUtlMemory {
public:
	T* GetBuffer( void ) {
		return m_pMemory;
	}

	int GetAllocationCount( void ) {
		return m_nAllocationCount;
	}

	int GetGrowSize( void ) {
		return m_nGrowSize;
	}

	T* OffsetBufferByIndex( size_t index ) {
		return m_pMemory + index;
	}

private:
	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template <typename T, typename Allocator = CUtlMemory<T>>
class CUtlVector {
public:
	Allocator GetMemory( void ) {
		return m_Memory;
	}

	int GetSize( void ) {
		return m_Size;
	}

	T* GetElements( void ) {
		return m_pElements;
	}

private:
	Allocator m_Memory;
	int m_Size;
	T* m_pElements;
};