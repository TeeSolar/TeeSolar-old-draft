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
 * THIS FILE HAS BEEN GENERATED BY A SCRIPT.
 * DO NOT EDIT MANUALLY!
 *
 * Please use the script "scripts/assets/assets.py" to regenerate it.
 *
 * Why this file is generated by a script?
 * Because there is more than 10 files that follow the same format.
 * In addition, each time a new member is added, enums, getter, setters,
 * copy/transfert functions and storage structure must be updated.
 * It's too much to avoid mistakes.
 */

#ifndef __CLIENT_ASSETS_SPRITE__
#define __CLIENT_ASSETS_SPRITE__

#include <shared/assets/asset.h>
#include <shared/assets/assetpath.h>

class CAsset_Sprite : public CAsset
{
public:
	enum
	{
		FLAG_FLIP_Y = (0x1 << 0),
		FLAG_FLIP_X = (0x1 << 1),
		FLAG_FLIP_ANIM_Y = (0x1 << 2),
		FLAG_FLIP_ANIM_X = (0x1 << 3),
		FLAG_SIZE_HEIGHT = (0x1 << 4),
	};
	
	static const int TypeId = 25;
	
	enum
	{
		NAME = CAsset::NAME,
		IMAGEPATH,
		X,
		Y,
		WIDTH,
		HEIGHT,
	};
	
	class CTuaType_0_2_0 : public CAsset::CTuaType_0_2_0
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_2_0& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_2_0& TuaType);
	};
	
	class CTuaType_0_2_1 : public CAsset::CTuaType_0_2_1
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_2_1& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_2_1& TuaType);
	};
	
	class CTuaType_0_2_2 : public CAsset::CTuaType_0_2_2
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_2_2& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_2_2& TuaType);
	};
	
	class CTuaType_0_2_3 : public CAsset::CTuaType_0_2_3
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_2_3& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_2_3& TuaType);
	};
	
	class CTuaType_0_2_4 : public CAsset::CTuaType_0_2_4
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_2_4& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_2_4& TuaType);
	};
	
	class CTuaType_0_3_0 : public CAsset::CTuaType_0_3_0
	{
	public:
		CAssetPath::CTuaType m_ImagePath;
		tua_int32 m_X;
		tua_int32 m_Y;
		tua_int32 m_Width;
		tua_int32 m_Height;
		static void Read(class CAssetsSaveLoadContext* pLoadingContext, const CTuaType_0_3_0& TuaType, CAsset_Sprite& SysType);
		static void Write(class CAssetsSaveLoadContext* pLoadingContext, const CAsset_Sprite& SysType, CTuaType_0_3_0& TuaType);
	};
	

private:
	CAssetPath m_ImagePath;
	int m_X;
	int m_Y;
	int m_Width;
	int m_Height;

public:
	int GetPixelWidth() const;
	int GetPixelHeight() const;
	virtual ~CAsset_Sprite() {}
	
	template<typename T>
	T GetValue(int ValueType, const CSubPath& SubPath, T DefaultValue) const
	{
		return CAsset::GetValue<T>(ValueType, SubPath, DefaultValue);
	}
	
	template<typename T>
	bool SetValue(int ValueType, const CSubPath& SubPath, T Value)
	{
		return CAsset::SetValue<T>(ValueType, SubPath, Value);
	}
	
	int AddSubItem(int Type, const CSubPath& SubPath);
	
	int AddSubItemAt(int Type, const CSubPath& SubPath, int Index);
	
	void DeleteSubItem(const CSubPath& SubPath);
	
	void RelMoveSubItem(CSubPath& SubPath, int RelMove);
	
	CAsset_Sprite();
	inline CAssetPath GetImagePath() const { return m_ImagePath; }
	
	inline int GetX() const { return m_X; }
	
	inline int GetY() const { return m_Y; }
	
	inline int GetWidth() const { return m_Width; }
	
	inline int GetHeight() const { return m_Height; }
	
	inline void SetImagePath(const CAssetPath& Value) { m_ImagePath = Value; }
	
	inline void SetX(int Value) { m_X = Value; }
	
	inline void SetY(int Value) { m_Y = Value; }
	
	inline void SetWidth(int Value) { m_Width = Value; }
	
	inline void SetHeight(int Value) { m_Height = Value; }
	
	void AssetPathOperation(const CAssetPath::COperation& Operation)
	{
		Operation.Apply(m_ImagePath);
	}
	
};

template<> int CAsset_Sprite::GetValue(int ValueType, const CSubPath& SubPath, int DefaultValue) const;
template<> bool CAsset_Sprite::SetValue(int ValueType, const CSubPath& SubPath, int Value);
template<> CAssetPath CAsset_Sprite::GetValue(int ValueType, const CSubPath& SubPath, CAssetPath DefaultValue) const;
template<> bool CAsset_Sprite::SetValue(int ValueType, const CSubPath& SubPath, CAssetPath Value);

#endif