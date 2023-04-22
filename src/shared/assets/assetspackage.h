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

#ifndef __SHARED_ASSETSPACKAGE__
#define __SHARED_ASSETSPACKAGE__

#include <generated/assets/allassets.h>
#include <shared/assets/assetslist.h>

struct CAssetState
{
	int m_NumUpdates;
	bool m_ListedInEditor;
	
	CAssetState()
	{
		m_NumUpdates = 0;
		m_ListedInEditor = false;
	};
};

//TAG_ASSETSVERSION
enum
{
	ASSETSVERSION_0_2_0=1,
	ASSETSVERSION_0_2_1,
	ASSETSVERSION_0_2_2,
	ASSETSVERSION_0_2_3,
	ASSETSVERSION_0_2_4,
	ASSETSVERSION_0_3_0,
	ASSETSVERSION_NOT_SUPPORTED,
	
	ASSETSVERSION_CURRENT = ASSETSVERSION_0_3_0,
};

class CAssetsPackage
{
public:
	struct CTuaType_Info
	{
		tua_uint32 m_AssetsVersion;
		tua_stringid m_Author;
		tua_stringid m_Credits;
		tua_stringid m_License;
		tua_stringid m_Version;
		//TODO: add credits
	};
	
	struct CTuaType_Dependency
	{
		tua_stringid m_PackageName;
		tua_uint32 m_PackageCrc;
	};
	
	enum
	{
		STATE_CREATED = 0,
		STATE_LOADED,
		STATE_JUST_LOADED,
	};
	
private:
	dynamic_string m_Name;
	dynamic_string m_Directory;
	uint32 m_Crc;
	int m_State;
	std::vector<int> Dependencies;
	bool m_ReadOnly;
	bool m_Edited;
	
	dynamic_string m_Author;
	dynamic_string m_Credits;
	dynamic_string m_License;
	dynamic_string m_Version;

	#define MACRO_ASSETTYPE(Name) CAssetsList<CAsset_##Name, CAssetState> m_##Name;
	#include <generated/assets/assetsmacro.h>
	#undef MACRO_ASSETTYPE

protected:
	template<typename ASSET>
	CAssetsList<ASSET, CAssetState>& GetList();
	
	template<typename ASSET>
	const CAssetsList<ASSET, CAssetState>& GetList() const;

public:
	CAssetsPackage();
	void InitAssetState(const CAssetState& State);
	void Load_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext);
	void Save_AssetsFile(class CAssetsSaveLoadContext* pLoadingContext);
	void DeleteAsset(const CAssetPath& Path);
	void AssetPathOperation(const CAssetPath::COperation& Operation);
	void SubPathOperation(const CAssetPath& Path, const CSubPath::COperation& Operation);
	
	inline bool IsLoaded() const { return m_State != STATE_CREATED; }
	inline bool IsJustLoaded() const { return m_State == STATE_JUST_LOADED; }
	inline void LoadingDone() { if(m_State == STATE_JUST_LOADED) m_State = STATE_LOADED; }
	
	inline bool IsReadOnly() const { return m_ReadOnly; }
	inline void SetReadOnly(bool Value) { m_ReadOnly = Value; }
	
	inline bool IsEdited() const { return m_Edited; }
	inline void SetEdited(bool Value) { m_Edited = Value; }
	
	inline const char* GetName() const { return m_Name.buffer(); }
	inline void SetName(const char* pName) { m_Name = pName; }
	inline const char* GetDirectory() const { return m_Directory.buffer(); }
	inline void SetDirectory(const char* pDirectory) { m_Directory = pDirectory; }
	
	inline const char* GetAuthor() const { return m_Author.buffer(); }
	inline const char* GetCredits() const { return m_Credits.buffer(); }
	inline const char* GetLicense() const { return m_License.buffer(); }
	inline const char* GetVersion() const { return m_Version.buffer(); }
	
	inline void SetAuthor(const char* pValue) { m_Author = pValue; }
	inline void SetCredits(const char* pValue) { m_Credits = pValue; }
	inline void SetLicense(const char* pValue) { m_License = pValue; }
	inline void SetVersion(const char* pValue) { m_Version = pValue; }
	
	template<typename ASSET>
	const ASSET* GetAsset(const CAssetPath& Path) const
	{
		return GetList<ASSET>().GetAsset(Path);
	}
	
	template<typename ASSET>
	ASSET* GetAsset(const CAssetPath& Path)
	{
		return GetList<ASSET>().GetAsset(Path);
	}
	
	template<typename ASSET>
	CAssetState* GetAssetState(const CAssetPath& Path)
	{
		return GetList<ASSET>().GetAssetState(Path);
	}
	
	template<typename ASSET>
	int GetNumAssets() const
	{
		return GetList<ASSET>().GetNumAssets();
	}
	
	template<typename ASSET>
	ASSET* NewAsset(class CAssetsManager* pAssetsManager, CAssetPath* pPath)
	{
		return GetList<ASSET>().NewAsset(pAssetsManager, pPath);
	}
	
	template<typename ASSET>
	bool FindAsset(const char* pAssetName, CAssetPath* pPath) const
	{
		return GetList<ASSET>().FindAsset(pAssetName, pPath);
	}
};

#define MACRO_ASSETTYPE(Name) template<> CAssetsList<CAsset_##Name, CAssetState>& CAssetsPackage::GetList();
#include <generated/assets/assetsmacro.h>
#undef MACRO_ASSETTYPE

#define MACRO_ASSETTYPE(Name) template<> const CAssetsList<CAsset_##Name, CAssetState>& CAssetsPackage::GetList() const;
#include <generated/assets/assetsmacro.h>
#undef MACRO_ASSETTYPE

#endif
