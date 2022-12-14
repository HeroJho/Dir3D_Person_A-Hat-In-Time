#include "stdafx.h"
#include "..\Public\MapManager.h"

#include "GameInstance.h"
#include "MapManager.h"
#include "ToolManager.h"
#include "DataManager.h"

#include "StaticModel.h"



IMPLEMENT_SINGLETON(CMapManager)

CMapManager::CMapManager()
{
}






void CMapManager::Make_PickedModel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);



	CStaticModel::STATICMODELDESC Desc;
	ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

	string sTemp = CMapManager::Get_Instance()->Get_PickedString();
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	CGameObject* pObj = nullptr;
	pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc);

	Add_Model((CStaticModel*)pObj);


	RELEASE_INSTANCE(CGameInstance);
}

void CMapManager::Delete_Model()
{
	CStaticModel* pTemp = Find_Model(m_sPickedCreatedString);
	if (nullptr == pTemp)
		return;

	pTemp->Set_Dead();
	m_StaticModels.erase(m_sPickedCreatedString);
}

void CMapManager::Delete_Model_All()
{
	for (auto& Model : m_StaticModels)
		Model.second->Set_Dead();
	m_StaticModels.clear();
}

void CMapManager::Add_Model(CStaticModel * pStaticModel)
{
	string sTag;
	if (GenTag(&sTag))
	{
		// 제대로 Gen이 안됐음 카운트 모자람
	}

	pStaticModel->Set_ModelNum(sTag);
	m_StaticModels.emplace(sTag, pStaticModel);
}

CStaticModel * CMapManager::Get_PickedCreatedModel()
{
	return Find_Model(m_sPickedCreatedString);
}

CStaticModel * CMapManager::Find_Model(string sTag)
{
	_tchar cTempTag[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(sTag.data(), cTempTag);

	map<string, CStaticModel*>::iterator iter = m_StaticModels.find(sTag);

	if (iter == m_StaticModels.end())
		return nullptr;

	return iter->second;
}



void CMapManager::Conv_PickedModel_To_Bin()
{

	CStaticModel* pStaticModel = Get_PickedCreatedModel();
	if (nullptr == pStaticModel)
		return;
	CModel* pModel = (CModel*)pStaticModel->Get_ComponentPtr(TEXT("Com_Model"));

	if (pModel->Get_IsBin())
		return;


	char cTemp[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pStaticModel->Get_ModelTag(), cTemp);


	CDataManager::Get_Instance()->Conv_Bin_Model(pModel, cTemp, CDataManager::DATA_NOEANIM);
}

void CMapManager::Save_MapData()
{
	CDataManager::Get_Instance()->Save_Map(m_iID);
}

void CMapManager::Load_MapData()
{
	// 기존 Obj를 지운다
	Delete_Model_All();

	CDataManager::DATA_MAP* pMapData = CDataManager::Get_Instance()->Load_Map(m_iID);
	if (nullptr == pMapData)
		return;

	for (_int i = 0; i < pMapData->iNumObj; ++i)
	{
		CDataManager::DATA_MAP_OBJ DataObj = pMapData->pObjDatas[i];

		CStaticModel::STATICMODELDESC Desc;
		ZeroMemory(&Desc, sizeof(CStaticModel::STATICMODELDESC));

		string sTemp = DataObj.cName;
		CToolManager::Get_Instance()->CtoTC(sTemp.data(), Desc.cModelTag);
		Desc.vPos = DataObj.vPos;
		Desc.vAngle = DataObj.vAngle;
		Desc.vScale = DataObj.vScale;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pObj = nullptr;
		pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StaticModel"), LEVEL_MAPTOOL, TEXT("Layer_Model"), &pObj, &Desc);

		RELEASE_INSTANCE(CGameInstance);

		Add_Model((CStaticModel*)pObj);

	}

}


bool CMapManager::GenTag(string* pOut)
{
	int iGenNum = 0;
	string sTemp;
	while (iGenNum < 500)
	{
		sTemp = "Model_" + std::to_string(iGenNum);

		if (!Find_Model(sTemp))
		{
			*pOut = sTemp;
			return true;
		}

		++iGenNum;
	}

	return false;
}





void CMapManager::Free()
{
	m_StaticModels.clear();
}
