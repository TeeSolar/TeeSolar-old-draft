/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#ifndef __SHARED_SYSTEM_STRING__
#define __SHARED_SYSTEM_STRING__

#include <cstdlib>
#include <shared/math/math.h>

/**
 * Change a character to its lower case version
 * @param char Character to transform
 */
char char_lower(char c);

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

/*
	Function: str_length
		Returns the length of a zero terminated string.

	Parameters:
		str - Pointer to the string.

	Returns:
		Length of string in bytes excluding the zero termination.
*/
int str_length(const char *str);

/*
	Function: str_format
		Performs printf formating into a buffer.

	Parameters:
		buffer - Pointer to the buffer to recive the formated string.
		buffer_size - Size of the buffer.
		format - printf formating string.
		... - Parameters for the formating.

	Remarks:
		- See the C manual for syntax for the printf formating string.
		- The strings are treated as zero-terminated strings.
		- Garantees that dst string will contain zero-termination.
*/
void str_format(char *buffer, int buffer_size, const char *format, ...);

/*
	Function: str_check_pathname
		Check if the string contains '..' (parent directory) paths.

	Parameters:
		str - String to check.

	Returns:
		Returns 0 if the path is valid, -1 otherwise.

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
bool str_check_pathname(const char* str);

/*
	Function: str_skip_to_whitespace
		Skips leading non-whitespace characters(all but ' ', '\t', '\n', '\r').

	Parameters:
		str - Pointer to the string.

	Returns:
		Pointer to the first whitespace character found
		within the string.

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
char *str_skip_to_whitespace(char *str);

/*
	Function: str_skip_whitespaces
		Skips leading whitespace characters(' ', '\t', '\n', '\r').

	Parameters:
		str - Pointer to the string.

	Returns:
		Pointer to the first non-whitespace character found
		within the string.

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
char *str_skip_whitespaces(char *str);

/*
	Function: str_timestamp
		Copies a time stamp in the format year-month-day_hour-minute-second to the string.

	Parameters:
		buffer - Pointer to a buffer that shall receive the time stamp string.
		buffer_size - Size of the buffer.

	Remarks:
		- Guarantees that buffer string will contain zero-termination.
*/
void str_timestamp(char *buffer, int buffer_size);

/*
	Function: str_utf8_check
		Checks if a strings contains just valid utf8 characters.

	Parameters:
		str - Pointer to a possible utf8 string.

	Returns:
		0 - invalid characters found.
		1 - only valid characters found.

	Remarks:
		- The string is treated as zero-terminated utf8 string.
*/
int str_utf8_check(const char *str);

/*
	Function: str_utf8_forward
		Moves a cursor forwards in an utf8 string

	Parameters:
		str - utf8 string
		cursor - position in the string

	Returns:
		New cursor position.

	Remarks:
		- Won't move the cursor beyond the zero termination marker
*/
int str_utf8_forward(const char *str, int cursor);

/*
	Function: str_utf8_rewind
		Moves a cursor backwards in an utf8 string

	Parameters:
		str - utf8 string
		cursor - position in the string

	Returns:
		New cursor position.

	Remarks:
		- Won't move the cursor less then 0
*/
int str_utf8_rewind(const char *str, int cursor);

int str_to_int(const char *str);
int str_to_int_base(const char *str, int base);

/*
	Function: str_find_nocase
		Finds a string inside another string case insensitive.

	Parameters:
		haystack - String to search in
		needle - String to search for

	Returns:
		A pointer into haystack where the needle was found.
		Returns NULL of needle could not be found.

	Remarks:
		- Only garanted to work with a-z/A-Z.
		- The strings are treated as zero-terminated strings.
*/
const char *str_find_nocase(const char *haystack, const char *needle);


/* FOREIGN CODE END: TeeWorlds ****************************************/

/**
 * Case sensitive comparison between strings
 * @param a the first string to compare
 * @param b the second string to compare
 * @return the function return 0 is both strings are equal, and <0 (>0)
 * 	if the first string is lesser (greater) than the second one.
 */
int str_comp(const char *a, const char *b);

/**
 * Case sensitive comparison between the first num caracters of two strings
 * @param a the first string to compare
 * @param b the second string to compare
 * @param num the maximum number of character to compare
 * @return the function return 0 is both strings are equal, and <0 (>0)
 * 	if the first string is lesser (greater) than the second one.
 */
int str_comp_num(const char *a, const char *b, int num);

/**
 * Copies a string to another
 * @param dst pointer to a buffer that shall receive the string
 * @param src string to be copied
 * @param dst_size size of the buffer dst
 */
void str_copy(char *dst, const char *src, int dst_size);

/**
 * Appends a string to another
 * @param dst pointer to a buffer that contains a string
 * @param src string to append
 * @param dst_size size of the buffer dst
 */
void str_append(char *dst, const char *src, int dst_size);

/**
 * Appends the first num character of a string to another
 * @param dst pointer to a buffer that contains a string
 * @param src string to append
 * @param dst_size size of the buffer dst
 * @param num the maximum number of character to append
 */
void str_append_num(char *dst, const char *src, int dst_size, int num);

//String contained in a rezisable array
template<int INITIALSIZE>
class _dynamic_string
{
private:
	char* m_pBuffer;
	int m_MaxSize;
	
	inline void copy(const char* pBuffer)
	{
		int Size = str_length(pBuffer)+1;
		if(Size > m_MaxSize)
			resize_buffer(Size);
		
		str_copy(m_pBuffer, pBuffer, m_MaxSize);
	}
	
	inline void copy(const _dynamic_string& s)
	{
		resize_buffer(s.m_MaxSize);
		str_copy(m_pBuffer, s.m_pBuffer, m_MaxSize);
	}
	
	inline void transfert(_dynamic_string& s)
	{
		if(m_pBuffer)
			delete[] m_pBuffer;
		
		m_pBuffer = s.m_pBuffer;
		m_MaxSize = s.m_MaxSize;
		s.m_pBuffer = NULL;
		s.m_MaxSize = 0;
	}

public:
	_dynamic_string() :
		m_pBuffer(NULL),
		m_MaxSize(0)
	{
		resize_buffer(INITIALSIZE);
	}
	
	explicit _dynamic_string(const char* pText) :
		m_pBuffer(NULL),
		m_MaxSize(0)
	{
		copy(pText);
	}
	
	_dynamic_string(const _dynamic_string& s) :
		m_pBuffer(NULL),
		m_MaxSize(0)
	{
		copy(s);
	}
	
	explicit _dynamic_string(_dynamic_string&& s) :
		m_pBuffer(NULL),
		m_MaxSize(0)
	{
		transfert(s);
	}
	
	~_dynamic_string()
	{
		if(m_pBuffer)
			delete[] m_pBuffer;
	}
	
	_dynamic_string& operator=(const char* pText)
	{
		copy(pText);
		return *this;
	}
	
	_dynamic_string& operator=(const _dynamic_string& s)
	{
		copy(s);
		return *this;
	}
	
	_dynamic_string& operator=(_dynamic_string&& s)
	{
		transfert(s);
		return *this;
	}
	
	void resize_buffer(int Size)
	{
		if(m_pBuffer)
		{
			char* pBuffer = new char[Size];
			str_copy(pBuffer, m_pBuffer, Size);
			delete[] m_pBuffer;
			m_pBuffer = pBuffer;
			m_MaxSize = Size;
		}
		else
		{
			m_MaxSize = Size;
			m_pBuffer = new char[m_MaxSize];
			m_pBuffer[0] = 0;
		}
	}

	inline char* buffer() { return m_pBuffer; }
	inline const char* buffer() const { return m_pBuffer; }
	inline int maxsize() const { return m_MaxSize; }
	
	inline int append_at(int Pos, const char* pBuffer)
	{
		int BufferSize = str_length(pBuffer);
		int Size = Pos+BufferSize+1;
		if(Size > m_MaxSize)
		{
			int NewSize = m_MaxSize*2;
			while(Size > NewSize)
				NewSize *= 2;
			
			resize_buffer(NewSize);
		}
		
		str_append(m_pBuffer+Pos, pBuffer, m_MaxSize-Pos);
		
		return min(Pos + BufferSize, m_MaxSize-1);
	}
	
	inline int append_at_num(int Pos, const char* pBuffer, int Num)
	{
		int Size = Pos+Num+1;
		if(Size > m_MaxSize)
		{
			int NewSize = m_MaxSize*2;
			while(Size > NewSize)
				NewSize *= 2;
			
			resize_buffer(NewSize);
		}
		
		str_append_num(m_pBuffer+Pos, pBuffer, m_MaxSize-Pos, Num);
		
		return min(Pos + Num, m_MaxSize-1);
	}
	
	inline void insert_at(int Pos, const char* pBuffer)
	{
		int Length = str_length(m_pBuffer);
		int BufferSize = str_length(pBuffer);
		int Size = Length+BufferSize+1;
		if(Size > m_MaxSize)
		{
			int NewSize = m_MaxSize*2;
			while(Size > NewSize)
				NewSize *= 2;
			
			resize_buffer(NewSize);
		}
		
		for(int c=Length-1; c>=Pos; c--)
			m_pBuffer[c+BufferSize] = m_pBuffer[c];
		for(int c=0; c<BufferSize; c++)
			m_pBuffer[Pos+c] = pBuffer[c];
		m_pBuffer[Length+BufferSize] = 0;
	}
	
	inline int length() const { return str_length(m_pBuffer); }
	inline void clear() { m_pBuffer[0] = 0; }
	inline bool empty() const { return (m_pBuffer[0] == 0); }
	
	inline void append(const _dynamic_string& String) { append_at(length(), String.buffer()); }
	inline void append(const char* pBuffer) { append_at(length(), pBuffer); }
	inline void append(const _dynamic_string& String, int num) { append_at_num(length(), String.buffer(), num); }
	inline void append(const char* pBuffer, int num) { append_at_num(length(), pBuffer, num); }
	bool operator<(const char* buffer) const
	{
		return (str_comp(m_pBuffer, buffer) < 0);
	}
	
	template<typename STR>
	bool operator<(const STR& str) const
	{
		return (str_comp(m_pBuffer, str.buffer()) < 0);
	}
	
	bool operator>(const char* buffer) const
	{
		return (str_comp(m_pBuffer, buffer) > 0);
	}

	template<typename STR>
	bool operator>(const STR& str) const
	{
		return (str_comp(m_pBuffer, str.buffer()) > 0);
	}
	
	bool operator==(const char* buffer) const
	{
		return (str_comp(m_pBuffer, buffer) == 0);
	}

	template<typename STR>
	bool operator==(const STR& str) const
	{
		return (str_comp(m_pBuffer, str.buffer()) == 0);
	}
	
	bool operator!=(const char* buffer) const
	{
		return (str_comp(m_pBuffer, buffer) != 0);
	}

	template<typename STR>
	bool operator!=(const STR& str) const
	{
		return (str_comp(m_pBuffer, str.buffer()) != 0);
	}

	int comp_num(const char* str, int num) const
	{
		return (str_comp_num(m_pBuffer, str, num) != 0);
	}

	template<typename STR>
	int comp_num(const STR& str, int num) const
	{
		return (str_comp_num(m_pBuffer, str.buffer(), num) != 0);
	}
};

typedef _dynamic_string<128> dynamic_string;

//Operations on strings

#endif
