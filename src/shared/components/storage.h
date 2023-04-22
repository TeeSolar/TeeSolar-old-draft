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

#ifndef __SHARED_STORAGE__
#define __SHARED_STORAGE__

#include <shared/system/string.h>
#include <shared/system/fs.h>
#include <shared/kernel.h>
#include <shared/math/vector.h>

#include <vector>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

inline void StrToInts(int *pInts, int Num, const char *pStr)
{
	int Index = 0;
	while(Num)
	{
		char aBuf[4] = {0,0,0,0};
		for(int c = 0; c < 4 && pStr[Index]; c++, Index++)
			aBuf[c] = pStr[Index];
		*pInts = ((aBuf[0]+128)<<24)|((aBuf[1]+128)<<16)|((aBuf[2]+128)<<8)|(aBuf[3]+128);
		pInts++;
		Num--;
	}

	// null terminate
	pInts[-1] &= 0xffffff00;
}

inline void IntsToStr(const int *pInts, int Num, char *pStr)
{
	while(Num)
	{
		pStr[0] = (((*pInts)>>24)&0xff)-128;
		pStr[1] = (((*pInts)>>16)&0xff)-128;
		pStr[2] = (((*pInts)>>8)&0xff)-128;
		pStr[3] = ((*pInts)&0xff)-128;
		pStr += 4;
		pInts++;
		Num--;
	}

	// null terminate
	pStr[-1] = 0;
}

inline vec4 IntsToColor(int r, int g, int b, int a)
{
	return vec4(
		static_cast<float>(r & 255)/255.0f,
		static_cast<float>(g & 255)/255.0f,
		static_cast<float>(b & 255)/255.0f,
		static_cast<float>(a & 255)/255.0f
	);
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

class CStorage : public CSharedKernel::CComponent
{
/* FOREIGN CODE BEGIN: TeeWorlds **************************************/
public:
	enum
	{
		TYPE_SAVE = 0,
		TYPE_DATA = 1,
		TYPE_ALL = -1,
		TYPE_ABSOLUTE = -2,
	};

protected:
	std::vector<dynamic_string> m_StoragePaths;
	std::vector<dynamic_string> m_DataDirs;
	dynamic_string m_SaveDir;
	dynamic_string m_CurrentDir;
	bool m_InitializeSaveDir;

protected:
	void FindDatadir(const char *pArgv0);
	void LoadPaths(const char *pArgv0);
	void AddPath(const char *pPath);
	const dynamic_string& GetPath(int Type, const char *pDir, dynamic_string& Path) const;
	
public:
	CStorage(CSharedKernel* pKernel);
	
	virtual bool InitConfig(int argc, const char** argv);
	virtual bool Init();
	
	IOHANDLE OpenFile(const char *pFilename, int Flags, int Type);
	IOHANDLE OpenFile(const char *pFilename, int Flags, int Type, dynamic_string& Path);
	void GetCompletePath(int Type, const char *pDir, dynamic_string& Buffer);
	
	int GetNumPaths() { return m_StoragePaths.size(); }
	
	bool FileExists(const char* pFilename, int Type);

/* FOREIGN CODE END: TeeWorlds ****************************************/
	
public:
	class CListDirIterator
	{
	private:
		CStorage* m_pStorage;
		const char* m_pDir;
		int m_Type;
		int m_TypeIter;
		fs_listdir_iterator* m_pIter;
		dynamic_string m_DirFullPath;
		
	public:		
		CListDirIterator(CStorage* pStorage, const char *pDir, int Type) :
			m_pStorage(pStorage),
			m_pDir(pDir),
			m_Type(Type),
			m_TypeIter(Type == TYPE_ALL ? 0 : Type),
			m_pIter(NULL)
		{
			
		}
		
		~CListDirIterator()
		{
			if(m_pIter)
				delete m_pIter;
		}
		
		bool Next()
		{
			bool GoNextIter = false;
			while(m_TypeIter < m_pStorage->GetNumPaths())
			{
				if(!m_pIter)
					m_pIter = fs_create_listdir_iterator(m_pStorage->GetPath(m_TypeIter, m_pDir, m_DirFullPath).buffer());
				
				if(!m_pIter)
					GoNextIter = true;
				else
				{
					bool Res = m_pIter->next();
					if(Res)
						return true;
					else
						GoNextIter = true;
				}
				
				if(GoNextIter)
				{
					if(m_pIter)
						delete m_pIter;
					m_pIter = NULL;
					if(m_Type == TYPE_ALL)
						m_TypeIter++;
					else 
						return false;
				}
			}
			return false;
		}
		
		void GetFilename(dynamic_string& Buffer)
		{
			if(m_pIter)
			{
				const char* pFilename = m_pIter->get_filename();
				
				int PathSize = m_DirFullPath.length();
				if(str_length(pFilename) > PathSize+1 && str_comp_num(pFilename, m_DirFullPath.buffer(), PathSize) == 0)
					Buffer.append(pFilename + PathSize +1);
				else
					Buffer.append(pFilename);
			}
		}
	};

	/**
	 * Lists the files in a directory
	 * @param dir Directory to list
	 * @return an iterator
	 */
	CListDirIterator* CreateListDirIterator(const char *pDir, int Type)
	{
		return new CListDirIterator(this, pDir, Type);
	}
};

#endif
