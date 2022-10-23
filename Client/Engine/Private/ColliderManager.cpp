#include "..\Public\ColliderManager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CColliderManager)

CColliderManager::CColliderManager()
{
}

void CColliderManager::Add_ColGroup(COLLIDERGROUP eGroup, CGameObject * pObj)
{
	if (nullptr == pObj)
		return;

	m_ObjGroups[eGroup].push_back(pObj);
	Safe_AddRef(pObj);
}

void CColliderManager::Calcul_ColGroup(COLLIDERGROUP eGroupL, COLLIDERGROUP eGroupR)
{
	for (auto& pObjL : m_ObjGroups[eGroupL])
	{
		for (auto& pObjR : m_ObjGroups[eGroupR])
		{
			list<CCollider*> LCols = pObjL->Get_Colliders();
			list<CCollider*> RCols = pObjR->Get_Colliders();


			for (auto& pColL : LCols)
			{
				for (auto& pColR : RCols)
				{
					if (pColL->Collision(pColR))
					{
						CCollider::OTHERTOMECOLDESC LDesc ,RDesc;
						LDesc.MyDesc = pColL->Get_Desc();
						LDesc.OtherDesc = pColR->Get_Desc();
						LDesc.pOther = pObjR;

						RDesc.MyDesc = pColR->Get_Desc();
						RDesc.OtherDesc = pColL->Get_Desc();
						RDesc.pOther = pObjL;

						pObjL->OnCollision(LDesc);
						pObjR->OnCollision(RDesc);
					}
				}
			}

		}
	}


}

void CColliderManager::Clear_ColGroup()
{

	for (_int i = 0; i < COLLIDER_END; ++i)
	{
		for (auto& pObj : m_ObjGroups[i])
			Safe_Release(pObj);
		m_ObjGroups[i].clear();
	}

}


void CColliderManager::Free()
{

	Clear_ColGroup();

}
