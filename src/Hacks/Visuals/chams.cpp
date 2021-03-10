#include "chams.hpp"
#include "../thirdperson.h"
#include "../AntiAim/antiaim.h"
#include "../TickManipulation/records.hpp"

IMaterial *materialChamsFlat, *materialChamsFlatIgnorez;
IMaterial *WhiteAdditive, *WhiteAdditiveIgnoreZ;
IMaterial *AdditiveTwo, *AdditiveTwoIgnoreZ;
IMaterial *materialChamsPearl;
IMaterial *materialChamsGlow;
IMaterial *materialChamsPulse;
IMaterial *GlowF;

Vector colro = Vector(0.5, 0, 1);

typedef void (*DrawModelExecuteFn)(void *, void *, void *, const ModelRenderInfo_t &, matrix3x4_t *);

static void
DrawPlayer(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	using namespace Settings::ESP;

	if (!FilterEnemy::Chams::enabled && !FilterLocalPlayer::RealChams::enabled && !FilterAlice::Chams::enabled) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer) {
		return;
	}

	ChamsType chamsType = ChamsType::NONE;
	C_BasePlayer *entity = (C_BasePlayer *) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
	    || entity->GetDormant()
	    || !entity->IsAlive()) {
		return;
	}

	if (Entity::IsTeamMate(entity, localplayer) && Settings::ESP::FilterAlice::Chams::enabled &&
	    entity != localplayer) {
		chamsType = Settings::ESP::FilterAlice::Chams::type;
	} else if (!Entity::IsTeamMate(entity, localplayer) && FilterEnemy::Chams::enabled) {
		chamsType = Settings::ESP::FilterEnemy::Chams::type;
	} else if (entity == localplayer && FilterLocalPlayer::RealChams::enabled) {
		chamsType = FilterLocalPlayer::RealChams::type;
	} else {
		return;
	}

	IMaterial *visible_material = nullptr;
	IMaterial *hidden_material = nullptr;

	switch (chamsType) {
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			visible_material = WhiteAdditive;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::FLAT:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::ADDITIVETWO:
			visible_material = AdditiveTwo;
			hidden_material = AdditiveTwoIgnoreZ;
			break;
		case ChamsType::PEARL:
			visible_material = materialChamsPearl;
			hidden_material = materialChamsPearl;
			break;
		case ChamsType::GLOW:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::GLOWF:
			visible_material = GlowF;
			hidden_material = materialChamsFlatIgnorez;
			break;
		default :
			return;
	}	

	if (entity == localplayer) {
		Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
		Color color = visColor;
		color *= 0.45f;

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
	} else if (Entity::IsTeamMate(entity, localplayer)) {
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::allyVisibleColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::allyColor.Color(entity).Value.w);
	} else if (!Entity::IsTeamMate(entity, localplayer)) {
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::enemyVisibleColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::enemyColor.Color(entity).Value.w);
	}

	
	if (entity->GetImmune()) {
		visible_material->AlphaModulate(0.5f);
		hidden_material->AlphaModulate(0.5f);
	}else {
		visible_material->AlphaModulate(1.f);
		hidden_material->AlphaModulate(1.f);
	}

	visible_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chamsType == ChamsType::WIREFRAME);
	visible_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, chamsType == ChamsType::NONE);
	hidden_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chamsType == ChamsType::WIREFRAME);
	hidden_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, chamsType == ChamsType::NONE);

	if (!Settings::ESP::Filters::legit && !Settings::ESP::Filters::visibilityCheck &&
	    !Entity::IsVisible(entity, CONST_BONE_HEAD, 180.f, true)) {
		modelRender->ForcedMaterialOverride(hidden_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
		modelRender->ForcedMaterialOverride(nullptr);
	} else {
		modelRender->ForcedMaterialOverride(visible_material);
	}

}

static void
DrawFake(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	if (!Settings::ESP::FilterLocalPlayer::Chams::enabled) {
		return;
	}
	if (!Settings::AntiAim::EnableFakAngle) {
		return;
	}
	if (!Settings::ThirdPerson::toggled) {
		return;
	}
	if (Settings::AntiAim::lbyBreak::Enabled) {
		return;
	}
	if (Settings::AntiAim::Jitter::Value > 0) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	C_BasePlayer *entity = (C_BasePlayer *) entityList->GetClientEntity(pInfo.entity_index);

	if (!entity
	    || entity != localplayer
	    || entity->GetDormant()
	    || !entity->IsAlive()) {
		return;
	}

	IMaterial *Fake_meterial = nullptr;

	switch (Settings::ESP::FilterLocalPlayer::Chams::type) {
		case ChamsType::WIREFRAME:
		case ChamsType::WHITEADDTIVE:
			Fake_meterial = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			Fake_meterial = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			Fake_meterial = AdditiveTwo;
			break;
		case ChamsType::PEARL:
			Fake_meterial = materialChamsPearl;
			break;
		case ChamsType::GLOW:
			Fake_meterial = materialChamsFlat;
			break;
		case ChamsType::GLOWF:
			Fake_meterial = GlowF;
			break;
		default:
			return;
	}

	Fake_meterial->AlphaModulate(1.f);

	Color fake_color = Color::FromImColor(Settings::ESP::Chams::FakeColor.Color(entity));
	Color color = fake_color;
	color *= 0.45f;

	Fake_meterial->ColorModulate(color);
	Fake_meterial->AlphaModulate(Settings::ESP::Chams::FakeColor.Color(entity).Value.w);

	static matrix3x4_t fakeBoneMatrix[128];
	float fakeangle = AntiAim::fakeAngle.y - AntiAim::realAngle.y;
	static Vector OutPos;
	// localplayer->SetupBones()
	for (int i = 0; i < 128; i++) {
		Math::AngleMatrix(Vector(0, fakeangle, 0), fakeBoneMatrix[i]);
		matrix::MatrixMultiply(fakeBoneMatrix[i], pCustomBoneToWorld[i]);
		Vector BonePos = Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3], pCustomBoneToWorld[i][2][3]) -
		                 pInfo.origin;
		Math::VectorRotate(BonePos, Vector(0, fakeangle, 0), OutPos);
		OutPos += pInfo.origin;
		fakeBoneMatrix[i][0][3] = OutPos.x;
		fakeBoneMatrix[i][1][3] = OutPos.y;
		fakeBoneMatrix[i][2][3] = OutPos.z;
	}

	if (entity->GetImmune()) {
		Fake_meterial->AlphaModulate(0.5f);
	}
	if (CreateMove::sendPacket) {
		static size_t _size = sizeof(matrix3x4_t) * 128;
		memcpy(Chams::BodyBoneMatrix, fakeBoneMatrix, _size);
	}

	//entity->SetupBones
	Fake_meterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME,
	                                  Settings::ESP::FilterLocalPlayer::Chams::type == ChamsType::WIREFRAME);
	Fake_meterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW,
	                                  Settings::ESP::FilterLocalPlayer::Chams::type == ChamsType::NONE);

	modelRender->ForcedMaterialOverride(Fake_meterial);
	modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, Chams::BodyBoneMatrix);
	modelRender->ForcedMaterialOverride(nullptr);
}

static void DrawWeapon(const ModelRenderInfo_t &pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled) {
		return;
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial *mat = nullptr;

	switch (Settings::ESP::Chams::Weapon::type) {
		case ChamsType::NONE:
		case ChamsType::WIREFRAME:
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			mat = AdditiveTwo;
			break;
		case ChamsType::PEARL:
			mat = materialChamsPearl;
			break;
		case ChamsType::GLOW:
			mat = materialChamsFlat;
			break;
		case ChamsType::GLOWF:
			mat = GlowF;
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Weapon::enabled) {
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);
	}

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawArms(const ModelRenderInfo_t &pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled) {
		return;
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial *mat = nullptr;

	switch (Settings::ESP::Chams::Arms::type) {
		case ChamsType::NONE:
		case ChamsType::WIREFRAME :
		case ChamsType::WHITEADDTIVE :
			mat = WhiteAdditive;
			break;
		case ChamsType::FLAT:
			mat = materialChamsFlat;
			break;
		case ChamsType::ADDITIVETWO:
			mat = AdditiveTwo;
			break;
		case ChamsType::PEARL:
			mat = materialChamsPearl;
			break;
		case ChamsType::GLOW:
			mat = materialChamsFlat;
			break;
		case ChamsType::GLOWF:
			mat = GlowF;
			break;
		default :
			return;
	}

	if (!Settings::ESP::Chams::Arms::enabled) {
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);
	}

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ChamsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ChamsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawBackTrack(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo)
{

	if (!Settings::BackTrack::enabled && !Settings::LagComp::enabled) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer) {
		return;
	}
	if (Records::SelectedRecords >= (int) Records::Ticks.size()) {
		return;
	}

	C_BasePlayer *entity = (C_BasePlayer *) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
	    || entity->GetDormant()
	    || !entity->IsAlive()
	    || entity == localplayer) {
		return;
	}

	static matrix3x4_t BacktrackBoneMatrix[128];
	static bool found = false;
	Records::TickInfo tick = Records::Ticks.at(Records::SelectedRecords);

	for (auto &Record : tick.records) {
		if (entity == Record.entity) {
			memcpy(BacktrackBoneMatrix, Record.bone_matrix, sizeof(matrix3x4_t) * 128);
			found = true;
			break;
		}
	}

	if (!found) {
		return;
	}
	found = false;

	Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
	visColor *= 0.5f;
	visColor.a = 255;

	IMaterial *visible_material = nullptr;

	// memcpy(visible_material, materialChamsFlatIgnorez, sizeof(visible_material));
	visible_material = materialChamsFlatIgnorez;

	visible_material->ColorModulate(visColor);
	visible_material->AlphaModulate(Settings::ESP::Chams::enemyVisibleColor.Color(entity).Value.w);

	// cvar->ConsoleDPrintf(XORSTR("Printing\n"));
	modelRender->ForcedMaterialOverride(visible_material);
	modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, BacktrackBoneMatrix);
	modelRender->ForcedMaterialOverride(nullptr);

}

void Chams::DrawModelExecute(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo,
                             matrix3x4_t *pCustomBoneToWorld)
{
	if (!engine->IsInGame()) {
		return;
	}

	if (!pInfo.pModel) {
		return;
	}

	static bool materialsCreated = false;
	if (!materialsCreated) {

		materialChamsPearl = material->FindMaterial(XORSTR("models/inventory_items/dogtags/dogtags_outline"),
		                                            TEXTURE_GROUP_MODEL);
		materialChamsGlow = Util::CreateMaterial3(XORSTR("VertexLitGeneric"), XORSTR("csgo/materials/glowOverlay.vmt"),
		                                          false, true, true, true, 1.f, colro);
		materialChamsPulse = Util::CreateMaterial4(XORSTR("VertexLitGeneric"),
		                                           XORSTR("models/inventory_items/dogtags/dogtags_outline"), false,
		                                           true, true, true, 1.f, colro);

		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true,
		                                         true, true, true);
		materialChamsFlatIgnorez = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), true,
		                                                true, true, true, true);

		WhiteAdditive = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false,
		                                     true, true, true);
		WhiteAdditiveIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true,
		                                            false, true, true, true);

		AdditiveTwo = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, false,
		                                   true, true, true, "models/effects/cube_white", "[1 1 1]", 1, "[0 1 1]");
		AdditiveTwoIgnoreZ = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false,
		                                          false, true, true, true, "models/effects/cube_white", "[1 1 1]", 1,
		                                          "[0 1 1]");

		GlowF = material->FindMaterial(XORSTR("vgui/achievements/glow"), TEXTURE_GROUP_MODEL);
		materialsCreated = true;
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);

	if (modelName.find(XORSTR("models/player")) != std::string::npos) {
		DrawFake(thisptr, context, state, pInfo, pCustomBoneToWorld);
		DrawBackTrack(thisptr, context, state, pInfo);
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
	else if (modelName.find(XORSTR("arms")) != std::string::npos) {
		DrawArms(pInfo);
	}
	else if (modelName.find(XORSTR("weapon")) != std::string::npos) {
		DrawWeapon(pInfo);
	}
}
