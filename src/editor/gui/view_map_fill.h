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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_FILL__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_FILL__

#include <editor/gui/view_map.h>
#include <generated/assets/maplayertiles.h>

class CCursorTool_MapFill : public CViewMap::CCursorTool
{	
protected:
	bool m_DragEnabled;
	vec2 m_Pivot;
	int m_Index;
	std::vector<int> m_ZoneDataInt;
	CAssetPath m_SelectedStyle;
	
protected:
	void AltButtonAction();
	
public:
	CCursorTool_MapFill(CViewMap* pViewMap);
	
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
	virtual void OnViewInputEvent(const CInput::CEvent& Event);
	
	void SetIndex(int Index) { m_Index = Index; }
	void SetZoneIndex(CAssetPath ZoneTypePath, int Index, const std::vector<int>& DataInt);
};

#endif
