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
 
#include <shared/system/fs.h>
#include <shared/system/string.h>
#include <shared/system/debug.h>
#include <shared/components/storage.h>
#include <zlib.h>

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

// compiled-in data-dir path
#define DATA_DIR "data"

CStorage::CStorage(CSharedKernel* pKernel) :
	CSharedKernel::CComponent(pKernel),
	m_InitializeSaveDir(true)
{
	SetName("Storage");
}

bool CStorage::InitConfig(int argc, const char** argv)
{
	// search for custom save directory
	{
		for(int i=1; i<argc; i++)
		{
			if(str_comp(argv[i], "--data-dir") == 0)
			{
				if(i+1<argc)
				{
					if(fs_is_dir(argv[i+1]))
					{
						m_DataDirs.emplace_back(argv[i+1]);
						i++;
					}
					else
					{
						debug::ErrorStream("Storage") << "Value specified with the --data-dir parameter is not a valid directory: " << argv[i+1] << std::endl;
						return false;
					}
				}
				else
				{
					debug::ErrorStream("Storage") << "Missing value for --data-dir parameter" << std::endl;
					return false;
				}
			}
			else if(str_comp(argv[i], "--save-dir") == 0)
			{
				if(i+1<argc)
				{
					if(fs_is_dir(argv[i+1]))
					{
						m_SaveDir = argv[i+1];
						i++;
					}
					else
					{
						debug::ErrorStream("Storage") << "Value specified with the --save-dir parameter is not a valid directory: " << argv[i+1] << std::endl;
						return false;
					}
				}
				else
				{
					debug::ErrorStream("Storage") << "Missing value for --save-dir parameter" << std::endl;
					return false;
				}
			}
			else if(str_comp(argv[i], "--save-dir-noinit") == 0)
			{
				m_InitializeSaveDir = false;
			}
		}
	}
	
	// get userdir
	if(m_SaveDir.empty())
		fs_storage_path("TeeSolar", m_SaveDir);
	
	// get datadir
	FindDatadir(argv[0]);
	
	return true;
}

bool CStorage::Init()
{
	m_StoragePaths.clear();
	
	if(!m_SaveDir.empty())
		m_StoragePaths.emplace_back(m_SaveDir);
	else
	{
		debug::ErrorStream("Storage") << "no save directory specified" << std::endl;
		return false;
	}
	
	if(m_DataDirs.size())
	{
		for(unsigned int i=0; i<m_DataDirs.size(); i++)
			m_StoragePaths.emplace_back(m_DataDirs[i]);
	}
	else
	{
		debug::ErrorStream("Storage") << "no data directory specified" << std::endl;
		return false;
	}
	
	//Initialize directories
	if(fs_makedir(m_StoragePaths[TYPE_SAVE].buffer()))
	{
		if(m_InitializeSaveDir)
		{
			dynamic_string Buf;
			
			fs_makedir(GetPath(TYPE_SAVE, "packages", Buf).buffer());
			fs_makedir(GetPath(TYPE_SAVE, "config", Buf).buffer());
			fs_makedir(GetPath(TYPE_SAVE, "maps", Buf).buffer());
		}
	}
	else
	{
		debug::ErrorStream("Storage") << "unable to create save directory" << m_StoragePaths[TYPE_SAVE].buffer() << std::endl;
		return false;
	}
	
	for(auto iter = m_StoragePaths.begin(); iter != m_StoragePaths.end(); ++iter)
		debug::InfoStream("Storage") << "Data Directory found: " << iter->buffer() << std::endl;

	return true;
}

void CStorage::LoadPaths(const char *pArgv0)
{
	// check current directory
	IOHANDLE File = io_open("config/storage.cfg", IOFLAG_READ);
	if(!File)
	{
		dynamic_string Filename(pArgv0);
		Filename.append("/config/storage.cfg");
		File = io_open(Filename.buffer(), IOFLAG_READ);

		if(!File)
		{
			debug::WarningStream("Storage") << "couldn't open config/storage.cfg" << std::endl;
			return;
		}
	}

	char *pLine;
	linereader LineReader(File);

	while((pLine = LineReader.get()))
	{
		if(str_length(pLine) > 9 && !str_comp_num(pLine, "add_path ", 9))
			AddPath(pLine+9);
	}

	io_close(File);

	if(m_StoragePaths.size() == 0)
		debug::WarningStream("Storage") << "no paths found in storage.cfg" << std::endl;
}

void CStorage::AddPath(const char *pPath)
{
	if(!pPath[0])
		return;
	
	if(fs_is_dir(pPath))
		m_StoragePaths.emplace_back(pPath);
}

void CStorage::FindDatadir(const char *pArgv0)
{
	// 1) use data-dir in PWD if present
	if(fs_is_dir("data/languages"))
	{
		m_DataDirs.emplace_back("data");
		return;
	}

	// 2) use compiled-in data-dir if present
	if(fs_is_dir(DATA_DIR"/languages"))
	{
		m_DataDirs.emplace_back(DATA_DIR);
		return;
	}

#if defined(CONF_FAMILY_UNIX)
	// 3) check for local locations
	{
		dynamic_string LocalPath;
		fs_home_path(LocalPath);
		LocalPath.append("/.local/share/teesoalr");
		
		dynamic_string TestPath = LocalPath;
		TestPath.append("/languages");
		if(fs_is_dir(TestPath.buffer()))
		{
			m_DataDirs.emplace_back(LocalPath.buffer());
			return;
		}
	}
	
	// 4) check for all default locations
	{
		const char *aDirs[] = {
			"/usr/share/teesoalr",
			"/usr/share/games/teesoalr",
			"/usr/local/share/teesoalr",
			"/usr/local/share/games/teesoalr",
			"/usr/pkg/share/teesoalr",
			"/usr/pkg/share/games/teesoalr",
			"/opt/teesoalr"
		};
		const int DirsCount = sizeof(aDirs) / sizeof(aDirs[0]);

		int i;
		for (i = 0; i < DirsCount; i++)
		{
			dynamic_string TestPath;
			TestPath.append(aDirs[i]);
			TestPath.append("/languages");
			
			if(fs_is_dir(TestPath.buffer()))
			{
				m_DataDirs.emplace_back(aDirs[i]);
				return;
			}
		}
	}
#endif
}

const dynamic_string& CStorage::GetPath(int Type, const char *pDir, dynamic_string& Path) const
{
	if(Type >= 0 && Type < static_cast<int>(m_StoragePaths.size()) && !m_StoragePaths[Type].empty())
	{
		Path = m_StoragePaths[Type];
		Path.append("/");
		Path.append(pDir);
	}
	else
		Path = pDir;
	
	return Path;
}

// Open a file. This checks that the path appears to be a subdirectory
// of one of the storage paths.
IOHANDLE CStorage::OpenFile(const char *pFilename, int Flags, int Type)
{
	dynamic_string FullPath;
	return OpenFile(pFilename, Flags, Type, FullPath);
}

IOHANDLE CStorage::OpenFile(const char *pFilename, int Flags, int Type, dynamic_string& FullPath)
{
	// Check whether the path contains '..' (parent directory) paths. We'd
	// normally still have to check whether it is an absolute path
	// (starts with a path separator (or a drive name on windows)),
	// but since we concatenate this path with another path later
	// on, this can't become absolute.
	//
	// E. g. "/etc/passwd" => "/path/to/storage//etc/passwd", which
	// is safe.
	if(!str_check_pathname(pFilename))
	{
		debug::WarningStream("Storage") << "OpenFile, check failed with " << pFilename << std::endl;
		return 0;
	}

	// open file
	if(Flags&IOFLAG_WRITE)
	{
		return io_open(GetPath(Type, pFilename, FullPath).buffer(), Flags);
	}
	else
	{
		if(Type == TYPE_ALL)
		{
			// check all available directories
			for(unsigned int i = 0; i < m_StoragePaths.size(); ++i)
			{
				IOHANDLE Handle = io_open(GetPath(i, pFilename, FullPath).buffer(), Flags);
				if(Handle)
					return Handle;
			}
		}
		else if(Type == TYPE_ABSOLUTE)
		{
			// check all available directories
			FullPath = pFilename;
			IOHANDLE Handle = io_open(pFilename, Flags);
			if(Handle)
				return Handle;
		}
		else if(Type >= 0 && Type < static_cast<int>(m_StoragePaths.size()))
		{
			// check wanted directory
			IOHANDLE Handle = io_open(GetPath(Type, pFilename, FullPath).buffer(), Flags);
			if(Handle)
				return Handle;
		}
	}

	return 0;
}

void CStorage::GetCompletePath(int Type, const char *pDir, dynamic_string& Buffer)
{
	if(Type == TYPE_ABSOLUTE)
		Buffer = pDir;
	else if(Type < 0 || Type >= static_cast<int>(m_StoragePaths.size()))
		Buffer.clear();
	else
		GetPath(Type, pDir, Buffer);
}

bool CStorage::FileExists(const char* pFilename, int Type)
{
	dynamic_string Buffer;
	GetPath(Type, pFilename, Buffer);
	
	IOHANDLE TestHandle = io_open(Buffer.buffer(), IOFLAG_READ);
	if(TestHandle)
	{
		io_close(TestHandle);
		return true;
	}
	else
		return false;
}

/* FOREIGN CODE END: TeeWorlds ****************************************/
