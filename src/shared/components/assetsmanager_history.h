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

#ifndef __SHARED_COMPONENTS_ASSETSMANAGER_HISTORY__
#define __SHARED_COMPONENTS_ASSETSMANAGER_HISTORY__

#include <shared/assets/assetpath.h>
#include <shared/assets/asset.h>

#include <memory>
#include <vector>

class CAssetsHistory
{
private:
	enum
	{
		MAXHISTORYSIZE = 128,
	};
	
	enum
	{
		OPERATION_EDITASSET=0,
		OPERATION_ADDASSET,
	};
	
	struct COperation
	{
		int m_Operation;
		CAssetPath m_AssetPath;
		std::unique_ptr<CAsset> m_pAsset;
	};
	
	struct CEntry
	{
		int m_Token;
		std::vector<COperation> m_Operations;
		
		CEntry() : m_Token(NO_TOKEN) { }
		void Reset();
	};
	
	class CAssetsManager* m_pAssetsManager;
	CEntry m_Entries[MAXHISTORYSIZE];
	int m_LastEntry;
	int m_Size;
	int m_LastToken;

public:
	enum
	{
		NO_TOKEN=-2,
		NEW_TOKEN=-1,
	};

private:
	void NewEntry(int Token);

public:
	CAssetsHistory(CAssetsManager* pAssetsManager);
	virtual ~CAssetsHistory();
	
	inline class CAssetsManager* AssetsManager() { return m_pAssetsManager; }
	
	void AddOperation_EditAsset(CAssetPath AssetPath, int Token);
	void AddOperation_AddAsset(CAssetPath AssetPath, int Token);
	
	void Flush();
	void Undo();
	int GenerateToken();
	inline int GetHistorySize() { return m_Size; }
};

#endif
