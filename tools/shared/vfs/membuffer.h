/* -------------------------------------------------------------------------------

   Copyright (C) 2022-2023 MRVN-Radiant and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of MRVN-Radiant.

   MRVN-Radiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MRVN-Radiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ------------------------------------------------------------------------------- */
#pragma once

#include <cstdio>
#include <cstdlib>
#include <utility>

typedef unsigned char byte;

//------------------------------------------------------------
// Purpose: Void pointer helper
//------------------------------------------------------------
class CVoidPtr
{
	void* ptr;
public:
	CVoidPtr(void* p) : ptr(p) {}
	template<typename T>
	operator T* () const {
		return static_cast<T*>(ptr);
	}
};

//------------------------------------------------------------
// Purpose: Data buffer wrapper
//------------------------------------------------------------
class CMemBuffer
{
	size_t m_size;
	byte* m_pData;

public:
	//------------------------------------------------------------
	// Purpose: Constructors
	//------------------------------------------------------------
	CMemBuffer() : m_pData(nullptr), m_size(0) {}

	explicit CMemBuffer(size_t size) : m_pData(new byte[size + 1]), m_size(size)
	{
		m_pData[m_size] = '\0';
	}

	CMemBuffer(CMemBuffer&& other) noexcept : m_pData(std::exchange(other.m_pData, nullptr)), m_size(other.m_size) {}


	//------------------------------------------------------------
	// Purpose: Destructor
	//------------------------------------------------------------
	~CMemBuffer() {
		delete[] m_pData;
	}

	//------------------------------------------------------------
	// Purpose: Returns pointer to buffer
	//------------------------------------------------------------
	CVoidPtr data() const {
		return m_pData;
	}
	
	//------------------------------------------------------------
	// Purpose: Returns size of buffer
	//------------------------------------------------------------
	size_t size() const {
		return m_size;
	}
	//------------------------------------------------------------
	// Purpose: bool operator
	//------------------------------------------------------------
	operator bool() const {
		return m_pData != nullptr;
	}

	//------------------------------------------------------------
	// Purpose: asignment operator
	//------------------------------------------------------------

	CMemBuffer& operator=(CMemBuffer&& other) noexcept {
		std::swap(m_pData, other.m_pData);
		std::swap(m_size, other.m_size);
		return *this;
	}
	//------------------------------------------------------------
	// Purpose: Delegates the ownership. Obtained buffer must be
	//          deallocated by delete[]
	//------------------------------------------------------------
	CVoidPtr release() {
		return std::exchange(m_pData, nullptr);
	}
};