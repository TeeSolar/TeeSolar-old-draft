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

#ifndef __CLIENT_ASSETSEDITOR_VIEWMAP_STAMP__
#define __CLIENT_ASSETSEDITOR_VIEWMAP_STAMP__

#include <editor/gui/view_map.h>
#include <generated/assets/maplayertiles.h>
#include <generated/assets/maplayerquads.h>
#include <generated/assets/maplayerobjects.h>
#include <generated/assets/mapzoneobjects.h>
#include <generated/assets/mapentities.h>

class CCursorTool_MapStamp : public CViewMap::CCursorTool
{
protected:
	bool m_DragEnabled;
	bool m_SelectionEnabled;
	array2d<int> m_IntDataSelection;
	array2d<CAsset_MapLayerTiles::CTile> m_TileSelection;
	std::vector<CAsset_MapLayerQuads::CQuad> m_QuadSelection;
	std::vector<CAsset_MapLayerObjects::CObject> m_LayerObjectsSelection;
	std::vector<CAsset_MapZoneObjects::CObject> m_ZoneObjectsSelection;
	std::vector<CAsset_MapEntities::CEntity> m_EntitySelection;
	vec2 m_Pivot;
	int m_Token;
	gui::CHListLayout* m_pOptions;
	
protected:
	void AltButtonAction();
	
public:
	CCursorTool_MapStamp(CViewMap* pViewMap);
	virtual void UpdateToolbar();
	virtual void OnViewButtonClick(int Button);
	virtual void OnViewButtonRelease(int Button);
	virtual void OnViewMouseMove();
	virtual void RenderView();
	virtual void Update(bool ParentEnabled);
	virtual void OnMouseMove();
	virtual void OnViewInputEvent(const CInput::CEvent& Event);
	void PaletteCallback_SelectImage(CAssetPath ImagePath, int MinX, int MinY, int MaxX, int MaxY);
	void PaletteCallback_SelectZoneType(CAssetPath ZoneTypePath, CSubPath Index, const std::vector<int>& DataInt);
	void PaletteCallback_SelectBrushType(CAssetPath MaterialPath, CSubPath Index);
	void PaletteCallback_SelectEntityType(CAssetPath ZoneTypePath);
	void VFlipSelection();
	void HFlipSelection();
	void RotateCWSelection();
	void RotateCCWSelection();
};

#endif
