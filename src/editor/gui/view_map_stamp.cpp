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

#include <editor/gui/view_map_stamp.h>
#include <editor/gui/image_picker.h>
#include <editor/gui/tilingmaterial_picker.h>
#include <editor/gui/zone_picker.h>
#include <editor/components/gui.h>
#include <client/components/assetsrenderer.h>
#include <client/gui/popup.h>
#include <client/gui/panellayout.h>
#include <client/gui/toggle.h>
#include <client/gui/integer-edit.h>
#include <client/gui/boxlayout.h>
#include <client/maprenderer.h>
#include <generated/assets/maplayertiles.h>
#include <generated/assets/mapzonetiles.h>
#include <shared/autolayer.h>

/* ENTITY PALETTE *****************************************************/

class CPopup_EntityPalette : public gui::CPopup
{
protected:
	class CEntityIcon : public gui::CWidget
	{
	protected:
		CGuiEditor* m_pAssetsEditor;
		CAssetPath m_EntityTypePath;
		array2d<CAsset_MapZoneTiles::CTile> m_Tiles;
		int m_Number;
		
	public:
		CEntityIcon(CGuiEditor *pAssetsEditor, CAssetPath EntityTypePath) :
			gui::CWidget(pAssetsEditor),
			m_pAssetsEditor(pAssetsEditor),
			m_EntityTypePath(EntityTypePath)
		{
			
		}
		
		virtual void UpdateBoundingSize()
		{
			m_BoundingSizeRect.BSMinimum(32.0f*4, 32.0f*3);
		}
		
		virtual void Render()
		{
			Graphics()->ClipPush(m_DrawRect.x, m_DrawRect.y, m_DrawRect.w, m_DrawRect.h);
			
			vec2 Pos = vec2(m_DrawRect.x + m_DrawRect.w/2, m_DrawRect.y + m_DrawRect.h/2);
			
			const CAsset_EntityType* pEntityType = AssetsManager()->GetAsset<CAsset_EntityType>(m_EntityTypePath);
			if(pEntityType)
				AssetsRenderer()->DrawSprite(pEntityType->GetGizmoPath(), Pos, 1.0f, 0.0f, 0x0, 1.0f);
			else
				AssetsRenderer()->DrawSprite(m_pAssetsEditor->m_Path_Sprite_GizmoPivot, Pos, 1.0f, 0.0f, 0x0, 1.0f);
			
			Graphics()->ClipPop();
		}
	};
	
	class CEntityButton : public gui::CButton
	{
	protected:
		CPopup_EntityPalette* m_pPopup;
		CCursorTool_MapStamp* m_pCursorTool;
		CAssetPath m_EntityTypePath;
		
	public:
		CEntityButton(CGuiEditor *pAssetsEditor, CCursorTool_MapStamp* pCursorTool, CPopup_EntityPalette* pPopup, CAssetPath EntityTypePath) :
			gui::CButton(pAssetsEditor, ""),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool),
			m_EntityTypePath(EntityTypePath)
		{
			const CAsset_EntityType* pEntityType = AssetsManager()->GetAsset<CAsset_EntityType>(EntityTypePath);
			if(pEntityType)
				SetText(pEntityType->GetName());
			
			SetButtonStyle(pAssetsEditor->m_Path_Button_PaletteIcon);
			SetIconWidget(new CEntityIcon(pAssetsEditor, EntityTypePath));
		}
		
		virtual void MouseClickAction()
		{
			m_pCursorTool->PaletteCallback_SelectEntityType(m_EntityTypePath);
			m_pPopup->Close();
		}
	};
	
public:
	CPopup_EntityPalette(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool, const gui::CRect& CreatorRect) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER)
	{
		SetBoxStyle(pAssetsEditor->m_Path_Box_Dialog);
		
		gui::CBoxLayout* pList = new gui::CBoxLayout(Context());
		Add(pList);		
		
		for(int p=0; p<AssetsManager()->GetNumPackages(); p++)
		{
			if(!AssetsManager()->IsValidPackage(p))
				continue;
			
			for(int i=0; i<AssetsManager()->GetNumAssets<CAsset_EntityType>(p); i++)
			{
				CAssetPath EntityPath = CAssetPath(CAsset_EntityType::TypeId, p, i);
				pList->Add(new CEntityButton(pAssetsEditor, pCursorTool, this, EntityPath));
			}
		}
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* ZONE PALETTE *******************************************************/

class CPopup_ZonePalette : public gui::CPopup
{
protected:
	class CPaletteZonePicker : public CZonePicker
	{
	protected:
		CPopup_ZonePalette* m_pPopup;
		CCursorTool_MapStamp* m_pCursorTool;
		
	protected:
		virtual void OnZonePicked(CSubPath IndexPath)
		{
			m_pCursorTool->PaletteCallback_SelectZoneType(m_ZoneTypePath, IndexPath, m_DataInt);
			m_pPopup->Close();
		}
	
	public:
		CPaletteZonePicker(CCursorTool_MapStamp* pCursorTool, CPopup_ZonePalette* pPopup, CAssetPath ZoneTypePath) :
			CZonePicker(pCursorTool->AssetsEditor(), ZoneTypePath, pCursorTool->ViewMap()->GetMemorizedZoneData(ZoneTypePath)),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	
public:
	CPopup_ZonePalette(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool, const gui::CRect& CreatorRect, CAssetPath ZoneTypePath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CPaletteZonePicker* pPicker = new CPaletteZonePicker(pCursorTool, this, ZoneTypePath);
		Add(pPicker);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* MATERIAL PALETTE ***************************************************/

class CPopup_MaterialPalette : public gui::CPopup
{
protected:
	class CPaletteMaterialPicker : public CTilingMaterialPicker
	{
	protected:
		CPopup_MaterialPalette* m_pPopup;
		CCursorTool_MapStamp* m_pCursorTool;
		
	protected:
		virtual void OnBrushPicked(CSubPath IndexPath)
		{
			m_pCursorTool->PaletteCallback_SelectBrushType(m_MaterialPath, IndexPath);
			m_pPopup->Close();
		}
	
	public:
		CPaletteMaterialPicker(CCursorTool_MapStamp* pCursorTool, CPopup_MaterialPalette* pPopup, CAssetPath ImagePath) :
			CTilingMaterialPicker(pCursorTool->AssetsEditor(), ImagePath),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	
public:
	CPopup_MaterialPalette(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool, const gui::CRect& CreatorRect, CAssetPath MaterialPath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CTilingMaterialPicker* pImagePicker = new CPaletteMaterialPicker(pCursorTool, this, MaterialPath);
		Add(pImagePicker);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* IMAGE PALETTE ******************************************************/

class CPopup_ImagePalette : public gui::CPopup
{
protected:
	class CPaletteImagePicker : public CImagePicker
	{
	protected:
		CPopup_ImagePalette* m_pPopup;
		CCursorTool_MapStamp* m_pCursorTool;
		
	protected:
		virtual void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY)
		{
			m_pCursorTool->PaletteCallback_SelectImage(m_ImagePath, MinX, MinY, MaxX, MaxY);
			m_pPopup->Close();
		}
	
	public:
		CPaletteImagePicker(CCursorTool_MapStamp* pCursorTool, CPopup_ImagePalette* pPopup, CAssetPath ImagePath) :
			CImagePicker(pCursorTool->AssetsEditor(), ImagePath),
			m_pPopup(pPopup),
			m_pCursorTool(pCursorTool)
		{
			
		}
	};
	
protected:
	CGuiEditor* m_pAssetsEditor;
	CAssetPath m_ImagePath;
	
public:
	CPopup_ImagePalette(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool, const gui::CRect& CreatorRect, CAssetPath ImagePath) :
		gui::CPopup(pAssetsEditor, CreatorRect, CreatorRect.w-16, CreatorRect.h-16, gui::CPopup::ALIGNMENT_INNER),
		m_pAssetsEditor(pAssetsEditor),
		m_ImagePath(ImagePath)
	{
		SetBoxStyle(m_pAssetsEditor->m_Path_Box_Dialog);
		
		CImagePicker* pImagePicker = new CPaletteImagePicker(pCursorTool, this, m_ImagePath);
		pImagePicker->SetSelectionType(CPaletteImagePicker::SELECTTYPE_AREA);
		Add(pImagePicker);
	}
	
	virtual void OnButtonClick(int Button)
	{
		if(m_DrawRect.IsInside(Context()->GetMousePos()) && m_VisibilityRect.IsInside(Context()->GetMousePos()) && Button == KEY_MOUSE_2)
			Close();
		
		gui::CPopup::OnButtonClick(Button);
	}

	virtual void OnInputEvent(const CInput::CEvent& Event)
	{
		if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
		{
			Close();
			return;
		}
		else
			CPopup::OnInputEvent(Event);
	}
	
	virtual int GetInputToBlock() { return CGui::BLOCKEDINPUT_ALL; }
};

/* FLIP AND ROTATE BUTTON *********************************************/

class CVFlipButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CCursorTool_MapStamp* m_pCursorTool;
	
protected:
	virtual void MouseClickAction() { m_pCursorTool->VFlipSelection(); }

public:
	CVFlipButton(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool) :
		gui::CButton(pAssetsEditor, "", pAssetsEditor->m_Path_Sprite_IconBigVFlip),
		m_pAssetsEditor(pAssetsEditor),
		m_pCursorTool(pCursorTool)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(_LSTRING("Flip vertically the current stamp selection"));
		
		gui::CButton::OnMouseMove();
	}
};

class CHFlipButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CCursorTool_MapStamp* m_pCursorTool;
	
protected:
	virtual void MouseClickAction() { m_pCursorTool->HFlipSelection(); }

public:
	CHFlipButton(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool) :
		gui::CButton(pAssetsEditor, "", pAssetsEditor->m_Path_Sprite_IconBigHFlip),
		m_pAssetsEditor(pAssetsEditor),
		m_pCursorTool(pCursorTool)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(_LSTRING("Flip horizontally the current stamp selection"));
		
		gui::CButton::OnMouseMove();
	}
};

class CRotateCCWButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CCursorTool_MapStamp* m_pCursorTool;
	
protected:
	virtual void MouseClickAction() { m_pCursorTool->RotateCCWSelection(); }

public:
	CRotateCCWButton(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool) :
		gui::CButton(pAssetsEditor, "", pAssetsEditor->m_Path_Sprite_IconBigRotateCCW),
		m_pAssetsEditor(pAssetsEditor),
		m_pCursorTool(pCursorTool)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(_LSTRING("Rotate counter-clockwise the current stamp selection"));
		
		gui::CButton::OnMouseMove();
	}
};

class CRotateCWButton : public gui::CButton
{
protected:
	CGuiEditor* m_pAssetsEditor;
	CCursorTool_MapStamp* m_pCursorTool;
	
protected:
	virtual void MouseClickAction() { m_pCursorTool->RotateCWSelection(); }

public:
	CRotateCWButton(CGuiEditor* pAssetsEditor, CCursorTool_MapStamp* pCursorTool) :
		gui::CButton(pAssetsEditor, "", pAssetsEditor->m_Path_Sprite_IconBigRotateCW),
		m_pAssetsEditor(pAssetsEditor),
		m_pCursorTool(pCursorTool)
	{
		
	}

	virtual void OnMouseMove()
	{
		if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
			m_pAssetsEditor->SetHint(_LSTRING("Rotate clockwise the current stamp selection"));
		
		gui::CButton::OnMouseMove();
	}
};

/* CURSOR TOOL ********************************************************/

CCursorTool_MapStamp::CCursorTool_MapStamp(CViewMap* pViewMap) :
	CCursorTool(pViewMap, _LSTRING("Stamp"), pViewMap->AssetsEditor()->m_Path_Sprite_IconStamp),
	m_pOptions(NULL)
{
	m_SelectionEnabled = false;
	m_DragEnabled = false;
}

void CCursorTool_MapStamp::UpdateToolbar()
{
	m_pOptions = new gui::CHListLayout(Context());
	m_pOptions->AddSeparator();
	{
		gui::CLabel* pLabel = new gui::CLabel(AssetsEditor(), _LSTRING("Actions:"));
		pLabel->NoTextClipping();
		m_pOptions->Add(pLabel, false);
	}
	m_pOptions->Add(new CRotateCCWButton(AssetsEditor(), this), false);
	m_pOptions->Add(new CRotateCWButton(AssetsEditor(), this), false);
	m_pOptions->Add(new CVFlipButton(AssetsEditor(), this), false);
	m_pOptions->Add(new CHFlipButton(AssetsEditor(), this), false);
	ViewMap()->GetToolbar()->Add(m_pOptions, false);
}

void CCursorTool_MapStamp::OnViewButtonClick(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(Button == KEY_MOUSE_1)
	{
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
		{
			if(m_SelectionEnabled)
			{
				const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					if(ViewMap()->GetGridAlign())
					{
						CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
						CursorMapPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorMapPos.x), floor(CursorMapPos.y))) + vec2(16.0f, 16.0f);
					}
					
					m_Token = AssetsManager()->GenerateToken();
					
					for(unsigned int i=0; i<m_QuadSelection.size(); i++)
					{
						CSubPath QuadPath = CAsset_MapLayerQuads::SubPath_Quad(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerQuads::TYPE_QUAD, m_Token));
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_PIVOT, m_QuadSelection[i].GetPivot() + CursorMapPos, m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_SIZE, m_QuadSelection[i].GetSize(), m_Token);
						AssetsManager()->SetAssetValue<float>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_ANGLE, m_QuadSelection[i].GetAngle(), m_Token);
						AssetsManager()->SetAssetValue<CAssetPath>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_ANIMATIONPATH, m_QuadSelection[i].GetAnimationPath(), m_Token);
						AssetsManager()->SetAssetValue<int64>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_ANIMATIONOFFSET, m_QuadSelection[i].GetAnimationOffset(), m_Token);
						AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_COLOR, m_QuadSelection[i].GetColor(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_VERTEX0, m_QuadSelection[i].GetVertex0(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_VERTEX1, m_QuadSelection[i].GetVertex1(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_VERTEX2, m_QuadSelection[i].GetVertex2(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_VERTEX3, m_QuadSelection[i].GetVertex3(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_UV0, m_QuadSelection[i].GetUV0(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_UV1, m_QuadSelection[i].GetUV1(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_UV2, m_QuadSelection[i].GetUV2(), m_Token);
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_UV3, m_QuadSelection[i].GetUV3(), m_Token);
						AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_COLOR0, m_QuadSelection[i].GetColor0(), m_Token);
						AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_COLOR1, m_QuadSelection[i].GetColor1(), m_Token);
						AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_COLOR2, m_QuadSelection[i].GetColor2(), m_Token);
						AssetsManager()->SetAssetValue<vec4>(AssetsEditor()->GetEditedAssetPath(), QuadPath, CAsset_MapLayerQuads::QUAD_COLOR3, m_QuadSelection[i].GetColor3(), m_Token);
					}
					
					m_Token = CAssetsHistory::NEW_TOKEN;
		
					CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
					pState->m_NumUpdates++;
				}
			}
			else
			{
				m_DragEnabled = true;
				vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
				m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		{
			if(m_SelectionEnabled)
			{
				CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset_Hard<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					if(ViewMap()->GetGridAlign())
					{
						CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
						CursorMapPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorMapPos.x), floor(CursorMapPos.y))) + vec2(16.0f, 16.0f);
					}
					
					m_Token = AssetsManager()->GenerateToken();
					
					AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token);
					for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
					{
						CSubPath ObjectPath = CAsset_MapLayerObjects::SubPath_Object(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapLayerObjects::TYPE_OBJECT, m_Token));
						pLayer->SetObject(ObjectPath, m_LayerObjectsSelection[i]);
						pLayer->SetObjectPosition(ObjectPath, m_LayerObjectsSelection[i].GetPosition() + CursorMapPos);
					}
					
					m_Token = CAssetsHistory::NEW_TOKEN;
		
					CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
					pState->m_NumUpdates++;
				}
			}	
			else
			{
				m_DragEnabled = true;
				vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
				m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
			}		
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		{
			if(m_SelectionEnabled)
			{
				CAsset_MapZoneObjects* pLayer = AssetsManager()->GetAsset_Hard<CAsset_MapZoneObjects>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					if(ViewMap()->GetGridAlign())
					{
						CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
						CursorMapPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorMapPos.x), floor(CursorMapPos.y))) + vec2(16.0f, 16.0f);
					}
					
					m_Token = AssetsManager()->GenerateToken();
					
					AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token);
					for(unsigned int i=0; i<m_ZoneObjectsSelection.size(); i++)
					{
						CSubPath ObjectPath = CAsset_MapZoneObjects::SubPath_Object(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapZoneObjects::TYPE_OBJECT, m_Token));
						pLayer->SetObject(ObjectPath, m_ZoneObjectsSelection[i]);
						pLayer->SetObjectPosition(ObjectPath, m_ZoneObjectsSelection[i].GetPosition() + CursorMapPos);
					}
					
					m_Token = CAssetsHistory::NEW_TOKEN;
		
					CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
					pState->m_NumUpdates++;
				}
			}	
			else
			{
				m_DragEnabled = true;
				vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
				m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
			}		
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
		{
			if(m_SelectionEnabled)
			{
				const CAsset_MapEntities* pLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					if(ViewMap()->GetGridAlign())
					{
						CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
						CursorMapPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorMapPos.x), floor(CursorMapPos.y))) + vec2(16.0f, 16.0f);
					}
					
					m_Token = AssetsManager()->GenerateToken();
					
					for(unsigned int i=0; i<m_EntitySelection.size(); i++)
					{
						CSubPath EntityPath = CAsset_MapEntities::SubPath_Entity(AssetsManager()->AddSubItem(AssetsEditor()->GetEditedAssetPath(), CSubPath::Null(), CAsset_MapEntities::TYPE_ENTITY, m_Token));
						AssetsManager()->SetAssetValue<vec2>(AssetsEditor()->GetEditedAssetPath(), EntityPath, CAsset_MapEntities::ENTITY_POSITION, m_EntitySelection[i].GetPosition() + CursorMapPos, m_Token);
						AssetsManager()->SetAssetValue<CAssetPath>(AssetsEditor()->GetEditedAssetPath(), EntityPath, CAsset_MapEntities::ENTITY_TYPEPATH, m_EntitySelection[i].GetTypePath(), m_Token);
					}
					
					m_Token = CAssetsHistory::NEW_TOKEN;
		
					CAssetState* pState = AssetsManager()->GetAssetState(AssetsEditor()->GetEditedAssetPath());
					pState->m_NumUpdates++;
				}
			}
			else
			{
				m_DragEnabled = true;
				vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
				m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
			}
		}
		else
		{
			m_DragEnabled = true;
			
			if(m_SelectionEnabled)
				m_Token = AssetsManager()->GenerateToken();
			
			vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
			m_Pivot = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
		}
	}
	else if(Button == KEY_MOUSE_2)
	{
		AltButtonAction();
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}

void CCursorTool_MapStamp::OnViewInputEvent(const CInput::CEvent& Event)
{
	if((Event.m_Key == KEY_SPACE) && (Event.m_Flags & CInput::FLAG_PRESS) && !Input()->IsTextEdited())
	{
		AltButtonAction();
		return;
	}
	else
		CCursorTool::OnViewInputEvent(Event);
}

void CCursorTool_MapStamp::AltButtonAction()
{
	if(m_SelectionEnabled)
		m_SelectionEnabled = false;
	else
	{
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
		{
			const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				Context()->DisplayPopup(new CPopup_ZonePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetZoneTypePath()));
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
		{
			const CAsset_MapEntities* pLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
				Context()->DisplayPopup(new CPopup_EntityPalette(AssetsEditor(), this, ViewMap()->GetViewRect()));
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer && pLayer->GetSourcePath().IsNull())
			{
				if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
					Context()->DisplayPopup(new CPopup_MaterialPalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetStylePath()));
				else if(pLayer->GetStylePath().GetType() == CAsset_Image::TypeId)
					Context()->DisplayPopup(new CPopup_ImagePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetStylePath()));
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
		{
			const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
			if(pLayer)
			{
				const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(pLayer->GetImagePath());
				if(pImage)
					Context()->DisplayPopup(new CPopup_ImagePalette(AssetsEditor(), this, ViewMap()->GetViewRect(), pLayer->GetImagePath()));
				else
				{
					m_QuadSelection.clear();
					
					m_QuadSelection.emplace_back();
					CAsset_MapLayerQuads::CQuad& Quad = m_QuadSelection.back();
					Quad.SetPivot(0.0f);
					
					Quad.SetVertex0(vec2(-128.0f, -128.0f));
					Quad.SetVertex1(vec2(128.0f, -128.0f));
					Quad.SetVertex2(vec2(-128.0f, 128.0f));
					Quad.SetVertex3(vec2(128.0f, 128.0f));
					
					Quad.SetUV0(vec2(0.0f, 0.0f));
					Quad.SetUV1(vec2(1.0f, 0.0f));
					Quad.SetUV2(vec2(0.0f, 1.0f));
					Quad.SetUV3(vec2(1.0f, 1.0f));
					
					m_SelectionEnabled = true;
				}
			}
		}
	}
}
	
void CCursorTool_MapStamp::OnViewButtonRelease(int Button)
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	if(m_DragEnabled)
	{
		m_DragEnabled = false;
		
		if(!m_SelectionEnabled)
		{
			vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
			
			if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
			{
				const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer && pLayer->GetSourcePath().IsNull())
				{
					vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
					vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
					int TileX0 = std::floor(PivotTilePos.x) - pLayer->GetPositionX();
					int TileY0 = std::floor(PivotTilePos.y) - pLayer->GetPositionY();
					int TileX1 = std::floor(CursorTilePos.x) - pLayer->GetPositionX();
					int TileY1 = std::floor(CursorTilePos.y) - pLayer->GetPositionY();
				
					int X0 = min(TileX0, TileX1);
					int Y0 = min(TileY0, TileY1);
					int X1 = max(TileX0, TileX1) + 1;
					int Y1 = max(TileY0, TileY1) + 1;
					
					if(Input()->KeyIsPressed(KEY_LSHIFT))
					{
						int Token = AssetsManager()->GenerateToken();
						
						for(int j=Y0; j<Y1; j++)
						{
							for(int i=X0; i<X1; i++)
							{
								CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
								AssetsManager()->SetAssetValue<int>(
									AssetsEditor()->GetEditedAssetPath(),
									TilePath,
									CAsset_MapLayerTiles::TILE_INDEX,
									1,
									Token
								);
								AssetsManager()->SetAssetValue<int>(
									AssetsEditor()->GetEditedAssetPath(),
									TilePath,
									CAsset_MapLayerTiles::TILE_FLAGS,
									0x0,
									Token
								);
								AssetsManager()->SetAssetValue<int>(
									AssetsEditor()->GetEditedAssetPath(),
									TilePath,
									CAsset_MapLayerTiles::TILE_BRUSH,
									1,
									Token
								);
							}
						}
			
						//Apply auto-tiling
						if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
							ApplyTilingMaterials_Layer(AssetsManager(), AssetsEditor()->GetEditedAssetPath(), X0, Y0, X1-X0, Y1-Y0, Token);
					}
					else
					{
						m_TileSelection.resize(X1-X0, Y1-Y0);
					
						for(int j=0; j<m_TileSelection.get_height(); j++)
						{
							for(int i=0; i<m_TileSelection.get_width(); i++)
							{
								CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(clamp(X0+i, 0, pLayer->GetTileWidth()-1), clamp(Y0+j, 0, pLayer->GetTileHeight()-1));
								m_TileSelection.get_clamp(i, j).SetIndex(pLayer->GetTileIndex(TilePath));
								m_TileSelection.get_clamp(i, j).SetFlags(pLayer->GetTileFlags(TilePath));
								m_TileSelection.get_clamp(i, j).SetBrush(pLayer->GetTileBrush(TilePath));
							}
						}
						
						m_SelectionEnabled = true;
					}
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
			{
				//Copy tiles
				const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
					vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
					int TileX0 = std::floor(PivotTilePos.x) - pLayer->GetPositionX();
					int TileY0 = std::floor(PivotTilePos.y) - pLayer->GetPositionY();
					int TileX1 = std::floor(CursorTilePos.x) - pLayer->GetPositionX();
					int TileY1 = std::floor(CursorTilePos.y) - pLayer->GetPositionY();
				
					int X0 = min(TileX0, TileX1);
					int Y0 = min(TileY0, TileY1);
					int X1 = max(TileX0, TileX1) + 1;
					int Y1 = max(TileY0, TileY1) + 1;
					
					if(Input()->KeyIsPressed(KEY_LSHIFT))
					{
						int Index = 1;
						
						const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(pLayer->GetZoneTypePath());
						if(pZoneType)
						{
							CAsset_ZoneType::CIteratorIndex Iter = pZoneType->BeginIndex();
							while(Iter != pZoneType->EndIndex())
							{
								if(pZoneType->IsValidIndex(*Iter) && pZoneType->GetIndexUsed(*Iter) && (*Iter).GetId() > 0)
								{
									Index = (*Iter).GetId();
									break;
								}
								++Iter;
							}
						}
						
						int Token = AssetsManager()->GenerateToken();
						
						for(int j=Y0; j<Y1; j++)
						{
							for(int i=X0; i<X1; i++)
							{
								CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
								AssetsManager()->SetAssetValue<int>(
									AssetsEditor()->GetEditedAssetPath(),
									TilePath,
									CAsset_MapZoneTiles::TILE_INDEX,
									Index,
									Token
								);
								AssetsManager()->SetAssetValue<int>(
									AssetsEditor()->GetEditedAssetPath(),
									TilePath,
									CAsset_MapZoneTiles::TILE_FLAGS,
									0x0,
									Token
								);
							}
						}
			
						//Update tile layers that use this zone as source
						for(int p=0; p<AssetsManager()->GetNumPackages(); p++)
						{
							for(int l=0; l<AssetsManager()->GetNumAssets<CAsset_MapLayerTiles>(p); l++)
							{
								CAssetPath AutoLayerPath = CAssetPath(CAsset_MapLayerTiles::TypeId, p, l);
								const CAsset_MapLayerTiles* pAutoLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AutoLayerPath);
								if(pAutoLayer && pAutoLayer->GetSourcePath() == AssetsEditor()->GetEditedAssetPath())
								{
									ApplyTilingMaterials_Layer(AssetsManager(), AutoLayerPath, X0, Y0, X1-X0, Y1-Y0, Token);
								}
							}
						}
					}
					else
					{
						m_TileSelection.resize(X1-X0, Y1-Y0);
						
						for(int j=0; j<m_TileSelection.get_height(); j++)
						{
							for(int i=0; i<m_TileSelection.get_width(); i++)
							{
								CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(clamp(X0+i, 0, pLayer->GetTileWidth()-1), clamp(Y0+j, 0, pLayer->GetTileHeight()-1));
								int Index = pLayer->GetTileIndex(TilePath);
								m_TileSelection.get_clamp(i, j).SetIndex(Index);
								m_TileSelection.get_clamp(i, j).SetBrush(Index);
							}
						}
						
						const array2d<int>& Data = pLayer->GetDataIntArray();
						if(Data.get_depth())
						{
							m_IntDataSelection.resize(X1-X0, Y1-Y0, Data.get_depth());
							
							for(int j=0; j<m_TileSelection.get_height(); j++)
							{
								for(int i=0; i<m_TileSelection.get_width(); i++)
								{
									for(int d=0; d<Data.get_depth(); d++)
									{
										m_IntDataSelection.get_clamp(i, j, d) = Data.get_clamp(clamp(X0+i, 0, pLayer->GetTileWidth()-1), clamp(Y0+j, 0, pLayer->GetTileHeight()-1), d);
									}
								}
							}
						}
						else
						{
							m_IntDataSelection.resize(X1-X0, Y1-Y0, 1);
							
							for(int j=0; j<m_TileSelection.get_height(); j++)
							{
								for(int i=0; i<m_TileSelection.get_width(); i++)
								{
									m_IntDataSelection.set_clamp(i, j, 0, 0);
								}
							}
						}
						
						m_SelectionEnabled = true;
					}
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
			{
				m_QuadSelection.clear();
				
				//Copy quads
				const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					
					float X0 = min(m_Pivot.x, CursorMapPos.x);
					float Y0 = min(m_Pivot.y, CursorMapPos.y);
					float X1 = max(m_Pivot.x, CursorMapPos.x);
					float Y1 = max(m_Pivot.y, CursorMapPos.y);
					
					bool QuadsFound = false;
					
					CAsset_MapLayerQuads::CIteratorQuad Iter;
					for(Iter = pLayer->BeginQuad(); Iter != pLayer->EndQuad(); ++Iter)
					{
						vec2 Position;
						matrix2x2 Transform;
						pLayer->GetQuadTransform(*Iter, ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
						
						if(Position.x >= X0 && Position.x <= X1 && Position.y >= Y0 && Position.y <= Y1)
						{
							m_QuadSelection.emplace_back();
							CAsset_MapLayerQuads::CQuad& Quad = m_QuadSelection.back();
							Quad = pLayer->GetQuad(*Iter);
							Quad.SetPivot(Quad.GetPivot() - vec2(X0 + X1, Y0 + Y1)/2.0f);
							QuadsFound = true;
						}
					}
					
					if(QuadsFound)
					{
						m_SelectionEnabled = true;
					}
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
			{
				m_LayerObjectsSelection.clear();
				
				const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					
					float X0 = min(m_Pivot.x, CursorMapPos.x);
					float Y0 = min(m_Pivot.y, CursorMapPos.y);
					float X1 = max(m_Pivot.x, CursorMapPos.x);
					float Y1 = max(m_Pivot.y, CursorMapPos.y);
					
					bool ObjectFound = false;
					
					CAsset_MapLayerObjects::CIteratorObject Iter;
					for(Iter = pLayer->BeginObject(); Iter != pLayer->EndObject(); ++Iter)
					{
						const typename CAsset_MapLayerObjects::CObject& Object = pLayer->GetObject(*Iter);
						vec2 Position;
						matrix2x2 Transform;
						Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
						
						if(Position.x >= X0 && Position.x <= X1 && Position.y >= Y0 && Position.y <= Y1)
						{
							m_LayerObjectsSelection.emplace_back();
							CAsset_MapLayerObjects::CObject& NewObject = m_LayerObjectsSelection.back();
							NewObject = Object;
							NewObject.SetPosition(Object.GetPosition() - vec2(X0 + X1, Y0 + Y1)/2.0f);
							ObjectFound = true;
						}
					}
					
					if(ObjectFound)
					{
						m_SelectionEnabled = true;
					}
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
			{
				m_ZoneObjectsSelection.clear();
				
				const CAsset_MapZoneObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneObjects>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					
					float X0 = min(m_Pivot.x, CursorMapPos.x);
					float Y0 = min(m_Pivot.y, CursorMapPos.y);
					float X1 = max(m_Pivot.x, CursorMapPos.x);
					float Y1 = max(m_Pivot.y, CursorMapPos.y);
					
					bool ObjectFound = false;
					
					CAsset_MapZoneObjects::CIteratorObject Iter;
					for(Iter = pLayer->BeginObject(); Iter != pLayer->EndObject(); ++Iter)
					{
						const typename CAsset_MapZoneObjects::CObject& Object = pLayer->GetObject(*Iter);
						vec2 Position;
						matrix2x2 Transform;
						Object.GetTransform(AssetsManager(), ViewMap()->MapRenderer()->GetTime(), &Transform, &Position);
						
						if(Position.x >= X0 && Position.x <= X1 && Position.y >= Y0 && Position.y <= Y1)
						{
							m_ZoneObjectsSelection.emplace_back();
							CAsset_MapZoneObjects::CObject& NewObject = m_ZoneObjectsSelection.back();
							NewObject = Object;
							NewObject.SetPosition(Object.GetPosition() - vec2(X0 + X1, Y0 + Y1)/2.0f);
							ObjectFound = true;
						}
					}
					
					if(ObjectFound)
					{
						m_SelectionEnabled = true;
					}
				}
			}
			else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
			{
				m_EntitySelection.clear();
				
				//Copy entities
				const CAsset_MapEntities* pLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
				if(pLayer)
				{
					vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
					
					float X0 = min(m_Pivot.x, CursorMapPos.x);
					float Y0 = min(m_Pivot.y, CursorMapPos.y);
					float X1 = max(m_Pivot.x, CursorMapPos.x);
					float Y1 = max(m_Pivot.y, CursorMapPos.y);
					
					bool EntitiesFound = false;
					
					CAsset_MapEntities::CIteratorEntity Iter;
					for(Iter = pLayer->BeginEntity(); Iter != pLayer->EndEntity(); ++Iter)
					{
						vec2 Position = pLayer->GetEntityPosition(*Iter);					
						if(Position.x >= X0 && Position.x <= X1 && Position.y >= Y0 && Position.y <= Y1)
						{
							m_EntitySelection.emplace_back();
							CAsset_MapEntities::CEntity& Entity = m_EntitySelection.back();
							Entity = pLayer->GetEntity(*Iter);
							Entity.SetPosition(Entity.GetPosition() - vec2(X0 + X1, Y0 + Y1)/2.0f);
							EntitiesFound = true;
						}
					}
					
					if(EntitiesFound)
					{
						m_SelectionEnabled = true;
					}
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
	
	m_Token = CAssetsHistory::NEW_TOKEN;
}
	
void CCursorTool_MapStamp::OnViewMouseMove()
{
	if(!ViewMap()->GetViewRect().IsInside(Context()->GetMousePos()))
		return;
	
	if(!m_SelectionEnabled || !m_DragEnabled)
		return;
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);

	int TileX = std::floor(CursorMapPos.x/32.0f);
	int TileY = std::floor(CursorMapPos.y/32.0f);
	
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer && pLayer->GetSourcePath().IsNull())
		{
			TileX -= pLayer->GetPositionX();
			TileY -= pLayer->GetPositionY();
			
			int MinX = clamp(TileX, 0, pLayer->GetTileWidth()-1);
			int MinY = clamp(TileY, 0, pLayer->GetTileHeight()-1);
			int MaxX = clamp(TileX + m_TileSelection.get_width(), 1, pLayer->GetTileWidth());
			int MaxY = clamp(TileY + m_TileSelection.get_height(), 1, pLayer->GetTileHeight());
			
			for(int j=MinY; j<MaxY; j++)
			{
				for(int i=MinX; i<MaxX; i++)
				{
					CSubPath TilePath = CAsset_MapLayerTiles::SubPath_Tile(i, j);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapLayerTiles::TILE_INDEX, m_TileSelection.get_clamp(i-TileX, j-TileY).GetIndex(), m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapLayerTiles::TILE_FLAGS, m_TileSelection.get_clamp(i-TileX, j-TileY).GetFlags(), m_Token);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapLayerTiles::TILE_BRUSH, m_TileSelection.get_clamp(i-TileX, j-TileY).GetBrush(), m_Token);
				}
			}
			
			//Apply auto-tiling
			if(pLayer->GetStylePath().GetType() == CAsset_TilingMaterial::TypeId)
				ApplyTilingMaterials_Layer(AssetsManager(), AssetsEditor()->GetEditedAssetPath(), MinX, MinY, MaxX-MinX, MaxY-MinY, m_Token);
		}
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
	{
		const CAsset_MapZoneTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
		if(pLayer)
		{
			TileX -= pLayer->GetPositionX();
			TileY -= pLayer->GetPositionY();
			
			int MinX = clamp(TileX, 0, pLayer->GetTileWidth()-1);
			int MinY = clamp(TileY, 0, pLayer->GetTileHeight()-1);
			int MaxX = clamp(TileX + m_TileSelection.get_width(), 1, pLayer->GetTileWidth());
			int MaxY = clamp(TileY + m_TileSelection.get_height(), 1, pLayer->GetTileHeight());
			
			for(int j=MinY; j<MaxY; j++)
			{
				for(int i=MinX; i<MaxX; i++)
				{
					CSubPath TilePath = CAsset_MapZoneTiles::SubPath_Tile(i, j);
					AssetsManager()->SetAssetValue<int>(AssetsEditor()->GetEditedAssetPath(), TilePath, CAsset_MapZoneTiles::TILE_INDEX, m_TileSelection.get_clamp(i-TileX, j-TileY).GetBrush(), m_Token);
				}
			}
			
			const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(pLayer->GetZoneTypePath());
			if(pZoneType && pZoneType->GetDataIntArraySize() > 0)
			{
				CAsset_MapZoneTiles* pLayerEditable = AssetsManager()->GetAsset_Hard<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
				array2d<int>& Data = pLayerEditable->GetDataIntArray();
				
				AssetsManager()->SaveAssetInHistory(AssetsEditor()->GetEditedAssetPath(), m_Token);
				
				if(Data.get_width() != pLayer->GetTileWidth() || Data.get_height() != pLayer->GetTileHeight() || Data.get_depth() != pZoneType->GetDataIntArraySize())
				{
					Data.resize(pLayer->GetTileWidth(), pLayer->GetTileHeight(), pZoneType->GetDataIntArraySize());
				}
				
				for(int j=MinY; j<MaxY; j++)
				{
					for(int i=MinX; i<MaxX; i++)
					{
						for(int d=0; d<Data.get_depth(); d++)
							Data.get_clamp(i, j, d) = m_IntDataSelection.get_clamp(i-TileX, j-TileY, d);
					}
				}
			}
			
			//Update tile layers that use this zone as source
			for(int p=0; p<AssetsManager()->GetNumPackages(); p++)
			{
				for(int l=0; l<AssetsManager()->GetNumAssets<CAsset_MapLayerTiles>(p); l++)
				{
					CAssetPath AutoLayerPath = CAssetPath(CAsset_MapLayerTiles::TypeId, p, l);
					const CAsset_MapLayerTiles* pAutoLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AutoLayerPath);
					if(pAutoLayer && pAutoLayer->GetSourcePath() == AssetsEditor()->GetEditedAssetPath())
					{
						ApplyTilingMaterials_Layer(AssetsManager(), AutoLayerPath, MinX, MinY, MaxX-MinX, MaxY-MinY, m_Token);
					}
				}
			}
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapStamp::RenderView()
{	
	CAssetPath ImagePath;
	vec4 Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
		if(!pLayer)
			return;
		
		ImagePath = pLayer->GetStylePath();
		Color = pLayer->GetColor();
	}
	
	ViewMap()->MapRenderer()->SetGroup(ViewMap()->GetMapGroupPath());
	
	double Time = Graphics()->GetFrameTime()/1000.0;
	Color.a *= 0.3f + 0.7f*(1.0f+cos(2.0f*Pi*Time))/2.0f;
	
	vec2 CursorPos = vec2(Context()->GetMousePos().x, Context()->GetMousePos().y);
	vec2 CursorMapPos = ViewMap()->MapRenderer()->ScreenPosToMapPos(CursorPos);
	vec2 CursorTilePos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
	vec2 PivotTilePos = ViewMap()->MapRenderer()->MapPosToTilePos(m_Pivot);
	
	if(m_SelectionEnabled)
	{
		if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
		{
			const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer || pLayer->GetSourcePath().IsNotNull())
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
			
			vec2 LayerPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorTilePos.x), floor(CursorTilePos.y)));
			ViewMap()->MapRenderer()->RenderTiles_Image(m_TileSelection, LayerPos, ImagePath, Color, false);
		
			vec2 LayerScreenPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos);
			vec2 LayerScreenPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos + vec2(m_TileSelection.get_width(), m_TileSelection.get_height())*32.0f);
			vec2 LayerScreenSize = LayerScreenPos1 - LayerScreenPos0;
			
			gui::CRect Rect;
			Rect.x = LayerScreenPos0.x;
			Rect.y = LayerScreenPos0.y;
			Rect.w = LayerScreenSize.x;
			Rect.h = LayerScreenSize.y;
			
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneTiles::TypeId)
		{
			vec2 LayerPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(CursorTilePos.x), floor(CursorTilePos.y)));
			const CAsset_MapZoneTiles* pZone = AssetsManager()->GetAsset<CAsset_MapZoneTiles>(AssetsEditor()->GetEditedAssetPath());
			if(!pZone)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
				
			ViewMap()->MapRenderer()->RenderTiles_Zone(
				pZone->GetZoneTypePath(),
				m_TileSelection,
				LayerPos,
				1.0f,
				false
			);
		
			vec2 LayerScreenPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos);
			vec2 LayerScreenPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(LayerPos + vec2(m_TileSelection.get_width(), m_TileSelection.get_height())*32.0f);
			vec2 LayerScreenSize = LayerScreenPos1 - LayerScreenPos0;
			
			gui::CRect Rect;
			Rect.x = LayerScreenPos0.x;
			Rect.y = LayerScreenPos0.y;
			Rect.w = LayerScreenSize.x;
			Rect.h = LayerScreenSize.y;
			
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
		{
			vec2 RenderPos = CursorMapPos;
			if(ViewMap()->GetGridAlign())
			{
				RenderPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				RenderPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(RenderPos.x), floor(RenderPos.y))) + vec2(16.0f, 16.0f);
			}
			
			const CAsset_MapLayerQuads* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerQuads>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
			
			if(m_QuadSelection.size())
			{
				ViewMap()->MapRenderer()->RenderQuads(
					&m_QuadSelection[0],
					m_QuadSelection.size(),
					RenderPos,
					pLayer->GetImagePath(), 
					Color
				);
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId)
		{
			vec2 RenderPos = CursorMapPos;
			if(ViewMap()->GetGridAlign())
			{
				RenderPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				RenderPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(RenderPos.x), floor(RenderPos.y))) + vec2(16.0f, 16.0f);
			}
			
			const CAsset_MapLayerObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerObjects>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
			
			if(m_LayerObjectsSelection.size())
			{
				for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
				{
					ViewMap()->MapRenderer()->RenderObject(
						m_LayerObjectsSelection[i],
						RenderPos,
						Color,
						false
					);
				}
			}
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId)
		{
			vec2 RenderPos = CursorMapPos;
			if(ViewMap()->GetGridAlign())
			{
				RenderPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				RenderPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(RenderPos.x), floor(RenderPos.y))) + vec2(16.0f, 16.0f);
			}
			
			const CAsset_MapZoneObjects* pLayer = AssetsManager()->GetAsset<CAsset_MapZoneObjects>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
			
			if(m_ZoneObjectsSelection.size())
				ViewMap()->MapRenderer()->RenderObjects_Zone(pLayer->GetZoneTypePath(), m_ZoneObjectsSelection, RenderPos, Color);
		}
		else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId)
		{
			vec2 RenderPos = CursorMapPos;
			if(ViewMap()->GetGridAlign())
			{
				RenderPos = ViewMap()->MapRenderer()->ScreenPosToTilePos(CursorPos);
				RenderPos = ViewMap()->MapRenderer()->TilePosToMapPos(vec2(floor(RenderPos.x), floor(RenderPos.y))) + vec2(16.0f, 16.0f);
			}
			
			const CAsset_MapEntities* pLayer = AssetsManager()->GetAsset<CAsset_MapEntities>(AssetsEditor()->GetEditedAssetPath());
			if(!pLayer)
			{
				ViewMap()->MapRenderer()->UnsetGroup();
				return;
			}
			
			if(m_EntitySelection.size())
			{
				CAsset_MapEntities::CIteratorEntity IterEntity;
				for(unsigned int i=0; i<m_EntitySelection.size(); i++)
				{
					vec2 Pos = ViewMap()->MapRenderer()->MapPosToScreenPos(RenderPos + m_EntitySelection[i].GetPosition());
					CAssetPath TypePath = m_EntitySelection[i].GetTypePath();
					
					const CAsset_EntityType* pEntityType = AssetsManager()->GetAsset<CAsset_EntityType>(TypePath);
					if(pEntityType)
						AssetsRenderer()->DrawSprite(pEntityType->GetGizmoPath(), Pos, 1.0f, 0.0f, 0x0, Color);
					else
						AssetsRenderer()->DrawSprite(AssetsEditor()->m_Path_Sprite_GizmoPivot, Pos, 1.0f, 0.0f, 0x0, Color);
				}
			}
		}
	}
	else if(m_DragEnabled)
	{
		if(
			AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId ||
			AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerObjects::TypeId ||
			AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapZoneObjects::TypeId ||
			AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapEntities::TypeId
		)
		{
			float X0 = min(m_Pivot.x, CursorMapPos.x);
			float Y0 = min(m_Pivot.y, CursorMapPos.y);
			float X1 = max(m_Pivot.x, CursorMapPos.x);
			float Y1 = max(m_Pivot.y, CursorMapPos.y);
			
			vec2 ScreenPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X0, Y0));
			vec2 ScreenPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X1, Y1));
			vec2 ScreenSize = ScreenPos1 - ScreenPos0;
			
			gui::CRect Rect;
			Rect.x = ScreenPos0.x;
			Rect.y = ScreenPos0.y;
			Rect.w = ScreenSize.x;
			Rect.h = ScreenSize.y;
		
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
		}
		else
		{
			int TileX0 = std::floor(PivotTilePos.x);
			int TileY0 = std::floor(PivotTilePos.y);
			int TileX1 = std::floor(CursorTilePos.x);
			int TileY1 = std::floor(CursorTilePos.y);
		
			int X0 = min(TileX0, TileX1);
			int Y0 = min(TileY0, TileY1);
			int X1 = max(TileX0, TileX1) + 1;
			int Y1 = max(TileY0, TileY1) + 1;
			
			vec2 SelectionRectPos0 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X0*32.0f, Y0*32.0f));
			vec2 SelectionRectPos1 = ViewMap()->MapRenderer()->MapPosToScreenPos(vec2(X1*32.0f, Y1*32.0f));
			vec2 SelectionRectSize = SelectionRectPos1 - SelectionRectPos0;
			
			gui::CRect Rect;
			Rect.x = SelectionRectPos0.x;
			Rect.y = SelectionRectPos0.y;
			Rect.w = SelectionRectSize.x;
			Rect.h = SelectionRectSize.y;
		
			AssetsRenderer()->DrawGuiRect(&Rect, AssetsEditor()->m_Path_Rect_Selection);
		}
	}
	
	ViewMap()->MapRenderer()->UnsetGroup();
}
	
void CCursorTool_MapStamp::Update(bool ParentEnabled)
{
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
			{
				Enable();
				
				const CAsset_MapLayerTiles* pLayer = AssetsManager()->GetAsset<CAsset_MapLayerTiles>(AssetsEditor()->GetEditedAssetPath());
				if(!pLayer || pLayer->GetSourcePath().IsNotNull())
					Editable(false);
				else
					Editable(true);
			}
			break;
		case CAsset_MapEntities::TypeId:
		case CAsset_MapZoneTiles::TypeId:
		case CAsset_MapZoneObjects::TypeId:
		case CAsset_MapLayerObjects::TypeId:
		case CAsset_MapLayerQuads::TypeId:
			Enable();
			Editable(true);
			break;
		default:
			Disable();
	}
	
	if(m_pOptions)
	{
		if(IsUsed() && m_SelectionEnabled)
			m_pOptions->Enable();
		else
			m_pOptions->Disable();
	}
	
	if(ParentEnabled && IsEnabled() && IsEditable())
	{
		if(AssetsEditor()->m_BindCall_ToolStamp > 0)
		{
			AssetsEditor()->m_BindCall_ToolStamp = 0;
			ViewMap()->SetCursorTool(this);
		}
		
		if(IsUsed())
		{
			while(AssetsEditor()->m_BindCall_ApplyHFlip > 0)
			{
				AssetsEditor()->m_BindCall_ApplyHFlip--;
				HFlipSelection();
			}
			while(AssetsEditor()->m_BindCall_ApplyVFlip > 0)
			{
				AssetsEditor()->m_BindCall_ApplyVFlip--;
				VFlipSelection();
			}
			while(AssetsEditor()->m_BindCall_ApplyCWRotation > 0)
			{
				AssetsEditor()->m_BindCall_ApplyCWRotation--;
				RotateCWSelection();
			}
			while(AssetsEditor()->m_BindCall_ApplyCCWRotation > 0)
			{
				AssetsEditor()->m_BindCall_ApplyCCWRotation--;
				RotateCCWSelection();
			}
		}
	}
	
	CCursorTool::Update(ParentEnabled);
}
	
void CCursorTool_MapStamp::PaletteCallback_SelectImage(CAssetPath ImagePath, int MinX, int MinY, int MaxX, int MaxY)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(ImagePath);
		if(pImage)
		{
			int Width = MaxX-MinX;
			int Height = MaxY-MinY;
			
			m_TileSelection.resize(Width, Height);
			
			for(int j=0; j<Height; j++)
			{
				for(int i=0; i<Width; i++)
				{
					int Index = (MinY + j) * pImage->GetGridWidth() + MinX + i;
					m_TileSelection.get_clamp(i, j).SetIndex(Index);
					m_TileSelection.get_clamp(i, j).SetFlags(0x0);
					m_TileSelection.get_clamp(i, j).SetBrush((Index != 0) ? 1 : 0);
				}
			}
		}
		else
		{
			m_TileSelection.resize(1, 1);
			m_TileSelection.get_clamp(0, 0).SetIndex(0);	
			m_TileSelection.get_clamp(0, 0).SetFlags(0x0);
			m_TileSelection.get_clamp(0, 0).SetBrush(0);
		}
		m_SelectionEnabled = true;
	}
	else if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerQuads::TypeId)
	{
		m_QuadSelection.clear();
	
		float ImageWidth = 128.0f;
		float ImageHeight = 128.0f;
		int GridWidth = 1;
		int GridHeight = 1;
		
		const CAsset_Image* pImage = AssetsManager()->GetAsset<CAsset_Image>(ImagePath);
		if(pImage)
		{
			ImageWidth = pImage->GetDataWidth() * pImage->GetTexelSize()/1024.0f;
			ImageHeight = pImage->GetDataHeight() * pImage->GetTexelSize()/1024.0f;
			GridWidth = pImage->GetGridWidth();
			GridHeight = pImage->GetGridHeight();
		}
		
		vec2 Size = vec2((MaxX - MinX) * ImageWidth / GridWidth, (MaxY - MinY) * ImageHeight / GridHeight);
	
		vec2 UVMin = vec2(static_cast<float>(MinX)/GridWidth, static_cast<float>(MinY)/GridHeight);
		vec2 UVMax = vec2(static_cast<float>(MaxX)/GridWidth, static_cast<float>(MaxY)/GridHeight);
		
		m_QuadSelection.emplace_back();
		CAsset_MapLayerQuads::CQuad& Quad = m_QuadSelection.back();
		Quad.SetPivot(0.0f);
		
		Quad.SetVertex0(vec2(-Size.x/4.0f, -Size.y/4.0f));
		Quad.SetVertex1(vec2(Size.x/4.0f, -Size.y/4.0f));
		Quad.SetVertex2(vec2(-Size.x/4.0f, Size.y/4.0f));
		Quad.SetVertex3(vec2(Size.x/4.0f, Size.y/4.0f));
		
		Quad.SetUV0(vec2(UVMin.x, UVMin.y));
		Quad.SetUV1(vec2(UVMax.x, UVMin.y));
		Quad.SetUV2(vec2(UVMin.x, UVMax.y));
		Quad.SetUV3(vec2(UVMax.x, UVMax.y));
		
		m_SelectionEnabled = true;
	}
}
	
void CCursorTool_MapStamp::PaletteCallback_SelectZoneType(CAssetPath ZoneTypePath, CSubPath Index, const std::vector<int>& DataInt)
{
	int Number = 0;
	
	const CAsset_ZoneType* pZoneType = AssetsManager()->GetAsset<CAsset_ZoneType>(ZoneTypePath);
	if(pZoneType && pZoneType->IsValidIndex(Index))
		Number = Index.GetId();
	
	m_TileSelection.resize(1, 1);
	m_TileSelection.get_clamp(0, 0).SetIndex(Number);
	m_TileSelection.get_clamp(0, 0).SetBrush(Number);
	m_TileSelection.get_clamp(0, 0).SetFlags(0x0);
	
	if(pZoneType && pZoneType->GetDataIntArraySize())
	{
		m_IntDataSelection.resize(1, 1, pZoneType->GetDataIntArraySize());
		for(unsigned int d=0; d<DataInt.size(); d++)
			m_IntDataSelection.set_clamp(1, 1, d, DataInt[d]);
		
		ViewMap()->MemorizeZoneData(ZoneTypePath, DataInt);
	}
		
	m_SelectionEnabled = true;
}
	
void CCursorTool_MapStamp::PaletteCallback_SelectBrushType(CAssetPath MaterialPath, CSubPath Index)
{
	if(AssetsEditor()->GetEditedAssetPath().GetType() == CAsset_MapLayerTiles::TypeId)
	{
		m_TileSelection.resize(1, 1);
		m_TileSelection.get_clamp(0, 0).SetIndex(0);	
		m_TileSelection.get_clamp(0, 0).SetFlags(0x0);
		m_TileSelection.get_clamp(0, 0).SetBrush(Index.GetId());
			
		m_SelectionEnabled = true;
	}
}
	
void CCursorTool_MapStamp::PaletteCallback_SelectEntityType(CAssetPath EntityTypePath)
{
	m_EntitySelection.clear();
	
	m_EntitySelection.emplace_back();
	CAsset_MapEntities::CEntity& Entity = m_EntitySelection.back();
	Entity.SetTypePath(EntityTypePath);
	Entity.SetPosition(0.0f);
	
	m_SelectionEnabled = true;
}

void CCursorTool_MapStamp::VFlipSelection()
{
	if(!m_SelectionEnabled)
		return;
	
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
		{
			CAsset_MapLayerTiles::CTile TmpTile;
			int Width = m_TileSelection.get_width();
			int Flags;
			for(int j=0; j<m_TileSelection.get_height(); j++)
			{
				int MaxWidth = Width/2;
				if(Width%2 == 1)
					MaxWidth++;
				
				for(int i=0; i<MaxWidth; i++)
				{
					CAsset_MapLayerTiles::CTile& Tile0 = m_TileSelection.get_clamp(i, j);
					CAsset_MapLayerTiles::CTile& Tile1 = m_TileSelection.get_clamp(Width-i-1, j);
					
					if(i != Width-i-1)
					{
						std::swap(Tile0, Tile1);
						
						Flags = Tile0.GetFlags();
						if(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
							Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP;
						else
							Flags ^= CAsset_MapLayerTiles::TILEFLAG_VFLIP;
						Tile0.SetFlags(Flags);
					}
						
					Flags = Tile1.GetFlags();
					if(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP;
					else
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_VFLIP;
					Tile1.SetFlags(Flags);
				}
			}
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			for(unsigned int i=0; i<m_QuadSelection.size(); i++)
			{
				m_QuadSelection[i].SetPivotX(-m_QuadSelection[i].GetPivotX());
				m_QuadSelection[i].SetSizeX(-m_QuadSelection[i].GetSizeX());
				m_QuadSelection[i].SetAngle(-m_QuadSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
			{
				m_LayerObjectsSelection[i].SetPositionX(-m_LayerObjectsSelection[i].GetPositionX());
				m_LayerObjectsSelection[i].SetSizeX(-m_LayerObjectsSelection[i].GetSizeX());
				m_LayerObjectsSelection[i].SetAngle(-m_LayerObjectsSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapZoneObjects::TypeId:
		{
			for(unsigned int i=0; i<m_ZoneObjectsSelection.size(); i++)
			{
				m_ZoneObjectsSelection[i].SetPositionX(-m_ZoneObjectsSelection[i].GetPositionX());
				m_ZoneObjectsSelection[i].SetSizeX(-m_ZoneObjectsSelection[i].GetSizeX());
				m_ZoneObjectsSelection[i].SetAngle(-m_ZoneObjectsSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapEntities::TypeId:
		{
			for(unsigned int i=0; i<m_EntitySelection.size(); i++)
			{
				m_EntitySelection[i].SetPositionX(-m_EntitySelection[i].GetPositionX());
			}
			break;
		}
	}
}

void CCursorTool_MapStamp::HFlipSelection()
{
	if(!m_SelectionEnabled)
		return;
	
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
		{
			CAsset_MapLayerTiles::CTile TmpTile;
			int Height = m_TileSelection.get_height();
			int Flags;
			for(int i=0; i<m_TileSelection.get_width(); i++)
			{
				int MaxHeight = Height/2;
				if(Height%2 == 1)
					MaxHeight++;
				
				for(int j=0; j<MaxHeight; j++)
				{
					CAsset_MapLayerTiles::CTile& Tile0 = m_TileSelection.get_clamp(i, j);
					CAsset_MapLayerTiles::CTile& Tile1 = m_TileSelection.get_clamp(i, Height-j-1);
					
					if(j != Height-j-1)
					{
						std::swap(Tile0, Tile1);
						
						Flags = Tile0.GetFlags();
						if(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
							Flags ^= CAsset_MapLayerTiles::TILEFLAG_VFLIP;
						else
							Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP;
						Tile0.SetFlags(Flags);
					}
						
					Flags = Tile1.GetFlags();
					if(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_VFLIP;
					else
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP;
					Tile1.SetFlags(Flags);
				}
			}
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			for(unsigned int i=0; i<m_QuadSelection.size(); i++)
			{
				m_QuadSelection[i].SetPivotY(-m_QuadSelection[i].GetPivotY());
				m_QuadSelection[i].SetSizeY(-m_QuadSelection[i].GetSizeY());
				m_QuadSelection[i].SetAngle(-m_QuadSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
			{
				m_LayerObjectsSelection[i].SetPositionY(-m_LayerObjectsSelection[i].GetPositionY());
				m_LayerObjectsSelection[i].SetSizeY(-m_LayerObjectsSelection[i].GetSizeY());
				m_LayerObjectsSelection[i].SetAngle(-m_LayerObjectsSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapZoneObjects::TypeId:
		{
			for(unsigned int i=0; i<m_ZoneObjectsSelection.size(); i++)
			{
				m_ZoneObjectsSelection[i].SetPositionY(-m_ZoneObjectsSelection[i].GetPositionY());
				m_ZoneObjectsSelection[i].SetSizeY(-m_ZoneObjectsSelection[i].GetSizeY());
				m_ZoneObjectsSelection[i].SetAngle(-m_ZoneObjectsSelection[i].GetAngle());
			}
			break;
		}
		case CAsset_MapEntities::TypeId:
		{
			for(unsigned int i=0; i<m_EntitySelection.size(); i++)
				m_EntitySelection[i].SetPositionY(-m_EntitySelection[i].GetPositionY());
			break;
		}
	}
}

void CCursorTool_MapStamp::RotateCCWSelection()
{
	if(!m_SelectionEnabled)
		return;
	
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
		{
			array2d<CAsset_MapLayerTiles::CTile> TmpSelection;
			TmpSelection.resize(m_TileSelection.get_height(), m_TileSelection.get_width());
			
			for(int j=0; j<m_TileSelection.get_height(); j++)
			{
				for(int i=0; i<m_TileSelection.get_width(); i++)
				{
					int I = j;
					int J = m_TileSelection.get_width()-i-1;
					
					CAsset_MapLayerTiles::CTile& Tile = TmpSelection.get_clamp(I, J);
					Tile = m_TileSelection.get_clamp(i, j);
					
					int Flags = Tile.GetFlags();
					if(!(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE))
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP|CAsset_MapLayerTiles::TILEFLAG_VFLIP;
					Flags ^= CAsset_MapLayerTiles::TILEFLAG_ROTATE;
					
					Tile.SetFlags(Flags);
				}
			}
			
			m_TileSelection = std::move(TmpSelection);
			
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			float Angle = -Pi/4.0f;
			for(unsigned int i=0; i<m_QuadSelection.size(); i++)
			{
				m_QuadSelection[i].SetPivot(rotate(m_QuadSelection[i].GetPivot(), Angle));
				m_QuadSelection[i].SetAngle(m_QuadSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			float Angle = -Pi/4.0f;
			for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
			{
				m_LayerObjectsSelection[i].SetPosition(rotate(m_LayerObjectsSelection[i].GetPosition(), Angle));
				m_LayerObjectsSelection[i].SetAngle(m_LayerObjectsSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapZoneObjects::TypeId:
		{
			float Angle = -Pi/4.0f;
			for(unsigned int i=0; i<m_ZoneObjectsSelection.size(); i++)
			{
				m_ZoneObjectsSelection[i].SetPosition(rotate(m_ZoneObjectsSelection[i].GetPosition(), Angle));
				m_ZoneObjectsSelection[i].SetAngle(m_ZoneObjectsSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapEntities::TypeId:
		{
			float Angle = -Pi/4.0f;
			for(unsigned int i=0; i<m_EntitySelection.size(); i++)
			{
				m_EntitySelection[i].SetPosition(rotate(m_EntitySelection[i].GetPosition(), Angle));
			}
			break;
		}
	}
}

void CCursorTool_MapStamp::RotateCWSelection()
{
	if(!m_SelectionEnabled)
		return;
	
	switch(AssetsEditor()->GetEditedAssetPath().GetType())
	{
		case CAsset_MapLayerTiles::TypeId:
		case CAsset_MapZoneTiles::TypeId:
		{
			array2d<CAsset_MapLayerTiles::CTile> TmpSelection;
			TmpSelection.resize(m_TileSelection.get_height(), m_TileSelection.get_width());
			
			for(int j=0; j<m_TileSelection.get_height(); j++)
			{
				for(int i=0; i<m_TileSelection.get_width(); i++)
				{
					int I = m_TileSelection.get_height()-j-1;
					int J = i;
					
					CAsset_MapLayerTiles::CTile& Tile = TmpSelection.get_clamp(I, J);
					Tile = m_TileSelection.get_clamp(i, j);
					
					int Flags = Tile.GetFlags();
					if(Flags&CAsset_MapLayerTiles::TILEFLAG_ROTATE)
						Flags ^= CAsset_MapLayerTiles::TILEFLAG_HFLIP|CAsset_MapLayerTiles::TILEFLAG_VFLIP;
					Flags ^= CAsset_MapLayerTiles::TILEFLAG_ROTATE;
					
					Tile.SetFlags(Flags);
				}
			}
			
			m_TileSelection = std::move(TmpSelection);
			
			break;
		}
		case CAsset_MapLayerQuads::TypeId:
		{
			float Angle = Pi/4.0f;
			for(unsigned int i=0; i<m_QuadSelection.size(); i++)
			{
				m_QuadSelection[i].SetPivot(rotate(m_QuadSelection[i].GetPivot(), Angle));
				m_QuadSelection[i].SetAngle(m_QuadSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapLayerObjects::TypeId:
		{
			float Angle = Pi/4.0f;
			for(unsigned int i=0; i<m_LayerObjectsSelection.size(); i++)
			{
				m_LayerObjectsSelection[i].SetPosition(rotate(m_LayerObjectsSelection[i].GetPosition(), Angle));
				m_LayerObjectsSelection[i].SetAngle(m_LayerObjectsSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapZoneObjects::TypeId:
		{
			float Angle = Pi/4.0f;
			for(unsigned int i=0; i<m_ZoneObjectsSelection.size(); i++)
			{
				m_ZoneObjectsSelection[i].SetPosition(rotate(m_ZoneObjectsSelection[i].GetPosition(), Angle));
				m_ZoneObjectsSelection[i].SetAngle(m_ZoneObjectsSelection[i].GetAngle() + Angle);
			}
			break;
		}
		case CAsset_MapEntities::TypeId:
		{
			float Angle = Pi/4.0f;
			for(unsigned int i=0; i<m_EntitySelection.size(); i++)
			{
				m_EntitySelection[i].SetPosition(rotate(m_EntitySelection[i].GetPosition(), Angle));
			}
			break;
		}
	}
}

void CCursorTool_MapStamp::OnMouseMove()
{
	if(m_VisibilityRect.IsInside(Context()->GetMousePos()))
		ViewMap()->AssetsEditor()->SetHint(_LSTRING("Stamp Tool: Create and copy objects. Use right click to open the palette."));
	
	CViewMap::CCursorTool::OnMouseMove();
}
