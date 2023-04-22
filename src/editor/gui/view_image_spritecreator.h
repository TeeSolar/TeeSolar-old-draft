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

#ifndef __CLIENT_ASSETSEDITOR_VIEWIMAGE_SPIRTECREATOR__
#define __CLIENT_ASSETSEDITOR_VIEWIMAGE_SPIRTECREATOR__

#include <editor/gui/view_image.h>

class CCursorTool_ImageSpriteCreator : public CViewImage::CCursorTool
{	
public:
	CCursorTool_ImageSpriteCreator(CViewImage* pViewMap);
	virtual void OnImagePicked(int MinX, int MinY, int MaxX, int MaxY);
};

#endif
