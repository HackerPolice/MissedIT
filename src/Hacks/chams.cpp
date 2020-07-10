#include "chams.h"
#include "thirdperson.h"
#include "antiaim.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../settings.h"
#include "../interfaces.h"
#include "lagcomp.h"
#include "../Hooks/hooks.h"

#include <thread>

IMaterial	*materialChamsFlat,// FLAT
			*WhiteAdditive, // ADDITIVE
			*DogClass, // Love This one Dogglass
			*achivements, //
			*bubble, 
			*snowflake,
			*gold;

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

static void DrawPlayer(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	using namespace Settings::ESP;
	if ( !Settings::ESP::Chams::enabled && !FilterEnemy::Chams::enabled && !FilterLocalPlayer::Chams::enabled)
		return;
	if (!Settings::ThirdPerson::toggled && Settings::ThirdPerson::enabled)
		return;
		
	ChamsType chamsType = ChamsType::NONE;
	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive())
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	if ( Entity::IsTeamMate(entity,localplayer) && Settings::ESP::FilterAlise::Chams::enabled && entity != localplayer)
		chamsType = Settings::ESP::FilterAlise::Chams::type;

	else if (!Entity::IsTeamMate(entity, localplayer) && FilterEnemy::Chams::enabled)
		chamsType = Settings::ESP::FilterEnemy::Chams::type;

	else if ( entity == localplayer && FilterLocalPlayer::RealChams::enabled)
		chamsType = FilterLocalPlayer::Chams::type;
	else
		return;
	

	IMaterial *visible_material = nullptr;
	

	switch (chamsType)
	{
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			visible_material = WhiteAdditive;
			break;
		case ChamsType::DogClass :
			visible_material = DogClass;
			break;
		case ChamsType::FLAT:
			visible_material = materialChamsFlat;
			break;
		case ChamsType::BUBBLE:
			visible_material = bubble;
			break;
		case ChamsType::GOLD:
			visible_material = gold;
			break;
		case ChamsType::Achivements:
			visible_material = achivements;
			break;
		case ChamsType::NONE :
			break;
		default :
			return;
		
	}

	visible_material->AlphaModulate(1.f);

	if (entity == localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
		Color color = visColor;
		color *= 0.45f;
		
		visible_material->ColorModulate(visColor);

		visible_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
	}
	else if (Entity::IsTeamMate(entity, localplayer) && entity != localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
	}
	else if (!Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
	}

	if (entity->GetImmune())
	{
		visible_material->AlphaModulate(0.5f);
	}

	visible_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chamsType == ChamsType::WIREFRAME);
	visible_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, chamsType == ChamsType::NONE);


	if (!Settings::ESP::Filters::legit && !Settings::ESP::Filters::visibilityCheck && !Entity::IsVisible(entity, CONST_BONE_HEAD, 180.f, true))
	{
		modelRender->ForcedMaterialOverride(visible_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}

	modelRender->ForcedMaterialOverride(visible_material);
	
}

static void DrawFake(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LegitAntiAim::enable)
		return;
	if (!Settings::ESP::FilterLocalPlayer::Chams::enabled)
		return;
	if (!Settings::ThirdPerson::enabled)
		return;
	if (!Settings::ThirdPerson::toggled && Settings::ThirdPerson::enabled)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;
	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive()
		|| entity != localplayer)
		return;

	IMaterial* Fake_meterial = nullptr;

	switch (Settings::ESP::FilterLocalPlayer::Chams::type)
	{
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			Fake_meterial = WhiteAdditive;
			break;
		case ChamsType::DogClass :
			Fake_meterial = DogClass;
			break;
		case ChamsType::FLAT:
			Fake_meterial = materialChamsFlat;
			break;
		case ChamsType::BUBBLE:
			Fake_meterial = bubble;
			break;
		case ChamsType::GOLD:
			Fake_meterial = gold;
			break;
		case ChamsType::Achivements:
			Fake_meterial = achivements;
			break;
		case ChamsType::NONE :
			break;
		default :
			return;		
	}

	if (entity->GetImmune())
		Fake_meterial->AlphaModulate(0.5f);
	else if (localplayer->GetVelocity().Length() != 0)
		Fake_meterial->AlphaModulate(0.2f);
	else
		Fake_meterial->AlphaModulate(1.f);
	
	/*
	 * Testing for chams in fake angle 
	 * Hope for best
	 */
	Color fake_color = Color::FromImColor(Settings::ESP::Chams::FakeColor.Color(entity));
	Color color = fake_color;
	color *= 0.45f;

	Fake_meterial->ColorModulate(fake_color);

	static matrix3x4_t fakeBoneMatrix[128];
	const float &fakeangle = AntiAim::fakeAngle.y-AntiAim::realAngle.y ;
	static Vector OutPos;

	if ( !(globalVars->tickcount%15) )
	{
		for (int i = 0; i < 128; i++)
		{
			Math::AngleMatrix(Vector(0, fakeangle, 0), fakeBoneMatrix[i]);
			matrix::MatrixMultiply(fakeBoneMatrix[i], pCustomBoneToWorld[i]);
			Vector BonePos = Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3], pCustomBoneToWorld[i][2][3]) - pInfo.origin;
			Math::VectorRotate(BonePos, Vector(0, fakeangle, 0), OutPos);
			OutPos += pInfo.origin;
                    fakeBoneMatrix[i][0][3] = OutPos.x;
                    fakeBoneMatrix[i][1][3] = OutPos.y;
                    fakeBoneMatrix[i][2][3] = OutPos.z;
			
		}
	}

	Fake_meterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::FilterLocalPlayer::Chams::type == ChamsType::WIREFRAME);
	Fake_meterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::FilterLocalPlayer::Chams::type == ChamsType::NONE);

	modelRender->ForcedMaterialOverride(Fake_meterial);
	modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, fakeBoneMatrix);	
	modelRender->ForcedMaterialOverride(nullptr);
}

static void DrawWeapon(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Weapon::type)
	{
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::DogClass :
			mat = DogClass;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::BUBBLE:
			mat = bubble;
			break;
		case ChamsType::GOLD:
			mat = gold;
			break;
		case ChamsType::Achivements:
			mat = achivements;
			break;
		case ChamsType::NONE :
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Weapon::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawArms(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled)
		return;

	if (Settings::ThirdPerson::toggled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = nullptr;

	switch(Settings::ESP::Chams::Arms::type)
	{
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::DogClass :
			mat = DogClass;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::BUBBLE:
			mat = bubble;
			break;
		case ChamsType::GOLD:
			mat = gold;
			break;
		case ChamsType::Achivements:
			mat = achivements;
			break;
		case ChamsType::NONE :
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Arms::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

void Chams::DrawModelExecute(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!engine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	if (!pInfo.pModel)
		return;

	static bool materialsCreated = false;
	if (!materialsCreated)
	{
		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		
		WhiteAdditive = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false, true, true, true);;
		
		DogClass = material->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_VGUI);
		
		achivements = material->FindMaterial("VGUI/achievements/glow", TEXTURE_GROUP_VGUI);
	
		snowflake = material->FindMaterial("dev/snowflake", TEXTURE_GROUP_VGUI);
		
		bubble = material->FindMaterial("effects/bubble", TEXTURE_GROUP_VGUI);
		
		gold = material->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_VGUI);
		
		materialsCreated = true;
		
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);

	if (modelName.find(XORSTR("models/player")) != std::string::npos)
	{
		DrawFake(thisptr, context, state, pInfo, pCustomBoneToWorld);
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
		
	else if (modelName.find(XORSTR("arms")) != std::string::npos)
		DrawArms(pInfo);
	else if (modelName.find(XORSTR("weapon")) != std::string::npos)
		DrawWeapon(pInfo);
	
	
}
