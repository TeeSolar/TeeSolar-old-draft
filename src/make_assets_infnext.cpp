/*
 * Copyright (C) 2023 RemakePower
 * 
 * This file is part of TeeSolar.
 * 
 * TeeSolar is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeSolar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeSolar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <shared/system/debug.h>
#include <shared/system/string.h>
#include <shared/kernel.h>
#include <shared/components/assetsmanager.h>
#include <shared/components/storage.h>
#include <client/loading_tools.h>

#include <cstdlib>

#define CREATE_SPRITE_PATH(path, packageid, name, image, x, y, w, h) {\
	CAsset_Sprite* pSprite = pKernel->AssetsManager()->NewAsset_Hard<CAsset_Sprite>(&path, packageid);\
	pSprite->SetName(name);\
	pSprite->SetImagePath(image);\
	pSprite->SetX(x);\
	pSprite->SetY(y);\
	pSprite->SetWidth(w);\
	pSprite->SetHeight(h);\
}

#define CREATE_TILINGMATERIAL_CONDITION_INDEX(relx, rely, index) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_INDEX);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, index);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

#define CREATE_TILINGMATERIAL_CONDITION_NOTINDEX(relx, rely, index) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_NOTINDEX);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_VALUE, index);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

#define CREATE_TILINGMATERIAL_CONDITION_NOBORDER(relx, rely) {\
	CSubPath CondPath = CAsset_TilingMaterial::SubPath_RuleCondition(RulePath.GetId(), pKernel->AssetsManager()->AddSubItem_Hard(MaterialPath, RulePath, CAsset_TilingMaterial::TYPE_RULE_CONDITION));\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_TYPE, CAsset_TilingMaterial::CONDITIONTYPE_NOBORDER);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSX, relx);\
	pKernel->AssetsManager()->SetAssetValue_Hard<int>(MaterialPath, CondPath, CAsset_TilingMaterial::RULE_CONDITION_RELPOSY, rely);\
}

int main(int argc, char* argv[])
{
	{
		CSharedKernel* pKernel = new CSharedKernel();
		if(!pKernel->Init(argc, (const char**) argv))
		{
			debug::ErrorStream("Kernel") << "unable to initialize shared kernel" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		int PackageId = -1;
		
		/* UNIV TEEWORLDS */
		PackageId = pKernel->AssetsManager()->NewPackage("infnext");
		pKernel->AssetsManager()->SetPackageAuthor(PackageId, "RemakePower");
		pKernel->AssetsManager()->SetPackageCredits(PackageId, "InfNext");
		pKernel->AssetsManager()->SetPackageLicense(PackageId, "CC-BY-SA 3.0");
		pKernel->AssetsManager()->SetPackageVersion(PackageId, "0.0.1");
		
		CAssetPath ImageZonesPath = CreateNewImage(pKernel, PackageId, "zones", "images/univ_infnext/zones.png", CStorage::TYPE_ALL, 16, 16, true, 0);
		CAssetPath ImageEntitiesPath = CreateNewImage(pKernel, PackageId, "entities", "images/univ_infnext/entities.png", CStorage::TYPE_ALL, 4, 4);
		pKernel->AssetsManager()->SetAssetValue_Hard<int>(ImageEntitiesPath, CSubPath::Null(), CAsset_Image::TEXELSIZE, 768);
		
		CAssetPath InfNextZoneTypePath;
		int GroupId_Death = -1;
		int GroupId_Infect = -1;

		//Zones
		{
			CSubPath SubPath;
			
			CAsset_ZoneType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_ZoneType>(&InfNextZoneTypePath, PackageId);
			pAsset->SetName("NextZones");
			pAsset->SetImagePath(ImageZonesPath);
			
			GroupId_Death = pAsset->AddGroup();
			GroupId_Infect = pAsset->AddGroup();
			
			pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Death), "Death");
			pAsset->SetGroup(CAsset_ZoneType::SubPath_Group(GroupId_Infect), "Infect");
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "None");

			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Death");
			pAsset->SetIndexDescription(SubPath, "Any player touching this zone will die");
			pAsset->SetIndexColor(SubPath, 1.0f);
			pAsset->SetIndexBorderIndex(SubPath, 1);
			pAsset->SetIndexBorderColor(SubPath, vec4(1.0f, 0.0f, 0.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Death);
			
			SubPath = CAsset_ZoneType::SubPath_Index(pAsset->AddIndex());
			pAsset->SetIndexTitle(SubPath, "Infected area");
			pAsset->SetIndexDescription(SubPath, "Any human touching this zone is immediately infected");
			pAsset->SetIndexBorderIndex(SubPath, 2);
			pAsset->SetIndexBorderColor(SubPath, vec4(11.0f/255.0f, 173.0f/255.0f, 0.0f, 1.0f));
			pAsset->SetIndexGroup(SubPath, GroupId_Infect);
		}
		
		//EntityType, Human Spawn
		{
			CAssetPath GizmoPath;
			CAssetPath AssetPath;
			
			CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizimoHumanSpawn", ImageEntitiesPath, 0, 3, 1, 1);
			
			CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
			pAsset->SetName("inHuman");
			pAsset->SetCollisionRadius(64.0f);
			pAsset->SetGizmoPath(GizmoPath);
		}
		//EntityType, Infect Spawn
		{
			CAssetPath GizmoPath;
			CAssetPath AssetPath;
			
			CREATE_SPRITE_PATH(GizmoPath, PackageId, "gizmoInfectSpawn", ImageEntitiesPath, 1, 3, 1, 1);
			
			CAsset_EntityType* pAsset = pKernel->AssetsManager()->NewAsset_Hard<CAsset_EntityType>(&AssetPath, PackageId);
			pAsset->SetName("inInfect");
			pAsset->SetCollisionRadius(64.0f);
			pAsset->SetGizmoPath(GizmoPath);
		}

		pKernel->AssetsManager()->Save_AssetsFile_SaveDir(PackageId);
		
		/* SHUTDOWN */
		pKernel->Shutdown();
	}

	exit(EXIT_SUCCESS);
}
