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

#ifndef __CLIENT_GUI_COMBOBOX__
#define __CLIENT_GUI_COMBOBOX__

#include "listlayout.h"
#include "button.h"
	
namespace gui
{

class CComboBox : public CButton
{
public:
	class CItem
	{
	public:
		CAssetPath m_IconPath;
		CLocalizableString m_Description;
	};
	
protected:
	CAssetPath m_ComboBoxStylePath;
	std::vector<CItem> m_EnumDescriptions;

protected:
	virtual void MouseClickAction();
	
	void RefreshComboBoxStyle();
	
public:
	CComboBox(class CGui *pConfig);
	void Add(const CLocalizableString& LString);
	void Add(const CLocalizableString& LString, const CAssetPath& IconPath);
	
	inline CAssetPath GetComboBoxStyle() const { return m_ComboBoxStylePath; }
	void SetComboBoxStyle(CAssetPath Path);
	
	virtual void Update(bool ParentEnabled);
	
	virtual int GetValue() const = 0;
	virtual void SetValue(int Value) = 0;
};

class CComboBoxString : public CButton
{
public:
	class CItem
	{
	public:
		CAssetPath m_IconPath;
		CLocalizableString m_Description;
	};
	
protected:
	CAssetPath m_ComboBoxStylePath;
	std::vector<CItem> m_EnumDescriptions;

protected:
	virtual void MouseClickAction();
	
	void RefreshComboBoxStyle();
	
public:
	CComboBoxString(class CGui *pConfig);
	void Add(const CLocalizableString& LString);
	void Add(const CLocalizableString& LString, const CAssetPath& IconPath);
	
	inline CAssetPath GetComboBoxStyle() const { return m_ComboBoxStylePath; }
	void SetComboBoxStyle(CAssetPath Path);
	
	virtual void Update(bool ParentEnabled);
	
	virtual dynamic_string GetValue() const = 0;
	virtual void SetValue(dynamic_string Value) = 0;
};

}

#endif
