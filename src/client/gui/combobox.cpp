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

#include <client/components/assetsrenderer.h>
#include <client/components/graphics.h>

#include "combobox.h"
#include "popup.h"
#include "listlayout.h"
	
namespace gui
{

class CEnumButton : public CButton
{
protected:
	int m_Value;
	dynamic_string m_ValueString;
	CPopup* m_pPopup;
	CComboBox* m_pComboBox;
	CComboBoxString* m_pComboBoxString;
	
protected:
	virtual void MouseClickAction()
	{
		if(m_pComboBox)
			m_pComboBox->SetValue(m_Value);
		else if(m_pComboBoxString)
			m_pComboBoxString->SetValue(m_ValueString);
		m_pPopup->Close();
	}	

public:
	CEnumButton(CGui *pContext, CComboBox* pComboBox, CPopup* pPopup, const CComboBox::CItem& Item, int Value) :
		CButton(pContext, Item.m_Description, Item.m_IconPath),
		m_Value(Value),
		m_pPopup(pPopup),
		m_pComboBox(pComboBox),
		m_pComboBoxString(0)
	{
		NoTextClipping();
	}

	CEnumButton(CGui *pContext, CComboBoxString* pComboBox, CPopup* pPopup, const CComboBoxString::CItem& Item, const char* Value) :
		CButton(pContext, Item.m_Description, Item.m_IconPath),
		m_ValueString(Value),
		m_pPopup(pPopup),
		m_pComboBox(0),
		m_pComboBoxString(pComboBox)
	{
		NoTextClipping();
	}
};

class CComboBoxPopup : public CPopup
{
public:
	CComboBoxPopup(CGui *pContext, CComboBox* pComboBox, const CRect& CreatorRect, const std::vector<CComboBox::CItem>& EnumDescriptions, CAssetPath ButtonStyle) :
		CPopup(pContext, CreatorRect, CreatorRect.w, -1, CPopup::ALIGNMENT_BOTTOM)
	{
		SetLevel(LEVEL_HIGHEST);
		
		CVListLayout* pLayout = new CVListLayout(Context());
		Add(pLayout);
		
		for(unsigned int i=0; i<EnumDescriptions.size(); i++)
		{
			CEnumButton* pButton = new CEnumButton(Context(), pComboBox, this, EnumDescriptions[i], i);
			pButton->SetButtonStyle(ButtonStyle);
			pLayout->Add(pButton, false);
		}
	}

	CComboBoxPopup(CGui *pContext, CComboBoxString* pComboBox, const CRect& CreatorRect, const std::vector<CComboBoxString::CItem>& EnumDescriptions, CAssetPath ButtonStyle) :
		CPopup(pContext, CreatorRect, CreatorRect.w, -1, CPopup::ALIGNMENT_BOTTOM)
	{
		SetLevel(LEVEL_HIGHEST);
		
		CVListLayout* pLayout = new CVListLayout(Context());
		Add(pLayout);
		
		for(unsigned int i=0; i<EnumDescriptions.size(); i++)
		{
			CEnumButton* pButton = new CEnumButton(Context(), pComboBox, this, EnumDescriptions[i], EnumDescriptions[i].m_Description.GetText());
			pButton->SetButtonStyle(ButtonStyle);
			pLayout->Add(pButton, false);
		}
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* COMBOBOX ***********************************************************/

CComboBox::CComboBox(CGui *pContext) :
	CButton(pContext, "")
{
	m_ComboBoxStylePath = Context()->GetComboBoxStyle();
}

void CComboBox::Add(const CLocalizableString& LString)
{
	m_EnumDescriptions.emplace_back();
	CItem& Item = m_EnumDescriptions.back();
	Item.m_Description = LString;
}

void CComboBox::Add(const CLocalizableString& LString, const CAssetPath& IconPath)
{
	m_EnumDescriptions.emplace_back();
	CItem& Item = m_EnumDescriptions.back();
	Item.m_Description = LString;
	Item.m_IconPath = IconPath;
}

void CComboBox::RefreshComboBoxStyle()
{
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
		SetButtonStyle(pStyle->GetButtonPath());
}
	
void CComboBox::Update(bool ParentEnabled)
{
	RefreshComboBoxStyle();
	
	int Value = GetValue();
	if(Value >= 0 && Value < static_cast<int>(m_EnumDescriptions.size()))
	{
		SetText(m_EnumDescriptions[Value].m_Description);
		SetIcon(m_EnumDescriptions[Value].m_IconPath);
	}
	else
	{
		SetText(_LSTRING("Unknown value"));
	}
	
	CButton::Update(ParentEnabled);
}

void CComboBox::MouseClickAction()
{
	CAssetPath ButtonStylePath;
	CAssetPath PopupStylePath;
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
	{
		ButtonStylePath = pStyle->GetEnumPath();
		PopupStylePath = pStyle->GetPopupPath();
	}
	
	CComboBoxPopup* pPopup = new CComboBoxPopup(Context(), this, m_DrawRect, m_EnumDescriptions, ButtonStylePath);
	pPopup->SetBoxStyle(PopupStylePath);
	
	Context()->DisplayPopup(pPopup);
}

/* COMBOBOX STRING ***********************************************************/

CComboBoxString::CComboBoxString(CGui *pContext) :
	CButton(pContext, "")
{
	m_ComboBoxStylePath = Context()->GetComboBoxStyle();
}

void CComboBoxString::Add(const CLocalizableString& LString)
{
	m_EnumDescriptions.emplace_back();
	CItem& Item = m_EnumDescriptions.back();
	Item.m_Description = LString;
}

void CComboBoxString::Add(const CLocalizableString& LString, const CAssetPath& IconPath)
{
	m_EnumDescriptions.emplace_back();
	CItem& Item = m_EnumDescriptions.back();
	Item.m_Description = LString;
	Item.m_IconPath = IconPath;
}

void CComboBoxString::RefreshComboBoxStyle()
{
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
		SetButtonStyle(pStyle->GetButtonPath());
}
	
void CComboBoxString::Update(bool ParentEnabled)
{
	RefreshComboBoxStyle();
	
	int Value = -1;

	for(unsigned i = 0;i < m_EnumDescriptions.size(); i ++)
	{
		if(str_comp(m_EnumDescriptions[i].m_Description.GetText(), GetValue().buffer()) == 0)
		{
			Value = i;
			break;
		}
	}

	if(Value >= 0 && Value < static_cast<int>(m_EnumDescriptions.size()))
	{
		SetText(m_EnumDescriptions[Value].m_Description);
		SetIcon(m_EnumDescriptions[Value].m_IconPath);
	}
	else
	{
		SetText(_LSTRING("Unknown value"));
	}
	
	CButton::Update(ParentEnabled);
}

void CComboBoxString::MouseClickAction()
{
	CAssetPath ButtonStylePath;
	CAssetPath PopupStylePath;
	const CAsset_GuiComboBoxStyle* pStyle = AssetsManager()->GetAsset<CAsset_GuiComboBoxStyle>(m_ComboBoxStylePath);
	if(pStyle)
	{
		ButtonStylePath = pStyle->GetEnumPath();
		PopupStylePath = pStyle->GetPopupPath();
	}
	
	CComboBoxPopup* pPopup = new CComboBoxPopup(Context(), this, m_DrawRect, m_EnumDescriptions, ButtonStylePath);
	pPopup->SetBoxStyle(PopupStylePath);
	
	Context()->DisplayPopup(pPopup);
}

}