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

#ifndef __CLIENT_GUI_BINDEDIT__
#define __CLIENT_GUI_BINDEDIT__

#include <client/components/bindsmanager.h>
#include "button.h"
	
namespace gui
{

class CAbstractBindEdit : public CAbstractLabel
{	
protected:
	bool m_Clicked;
	bool m_MouseOver;
	bool m_CatchInput;
	bool m_AcceptModifier;
	CAssetPath m_ButtonStylePath;
	
protected:
	CAbstractBindEdit(class CGui *pConfig);
	
	virtual CBindsManager::CKey GetValue() = 0;
	virtual void SetValue(CBindsManager::CKey Key) = 0;
	
public:
	virtual void Update(bool ParentEnabled);
	virtual void OnButtonClick(int Button);
	virtual void OnButtonRelease(int Button);
	virtual void OnMouseMove();
	virtual void OnInputEvent(const CInput::CEvent& Event);
	
	virtual void OnFocusStart();
	virtual void OnFocusStop();
	virtual int GetInputToBlock();
	
	void RefreshLabelStyle();
	inline CAssetPath GetButtonStyle() const { return m_ButtonStylePath; }
	void SetButtonStyle(CAssetPath Path);
};

class CBindEdit : public CAbstractBindEdit
{
protected:
	dynamic_string m_Command;
	int m_BindNum;

protected:
	virtual CBindsManager::CKey GetValue();
	virtual void SetValue(CBindsManager::CKey Key);

public:
	CBindEdit(class CGui *pConfig, const char* pCommand, int BindNum, bool Modifier);
};

}

#endif
