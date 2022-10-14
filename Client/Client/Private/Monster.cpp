#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}






HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(88.f, 0.f, 3.75f, 1.f));

	Set_State(MONSTER_CHASE);

	return S_OK;
}







void CMonster::Set_State(MONSTER_STATE eState)
{
	m_eState = eState;

	Set_Anim();
}

void CMonster::Set_Anim()
{

	switch (m_eState)
	{
	case Client::CMonster::MONSTER_IDLE:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CMonster::MONSTER_MOVE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CMonster::MONSTER_CHASE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CMonster::MONSTER_ATTACKED:
		break;
	case Client::CMonster::MONSTER_DIE:
		m_pModelCom->Set_AnimIndex(0);
		break;
	}

}






void CMonster::Tick(_float fTimeDelta)
{

	switch (m_eState)
	{
	case Client::CMonster::MONSTER_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_MOVE:
		Tick_Move(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_CHASE:
		Tick_Chase(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_ATTACKED:
		Tick_Attacked(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_DIE:
		Tick_Die(fTimeDelta);
		break;
	}

}

void CMonster::Tick_Idle(_float fTimeDelta)
{
}

void CMonster::Tick_Move(_float fTimeDelta)
{
}

void CMonster::Tick_Chase(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pTran = (CTransform*)pPlayer->Get_ComponentPtr(TEXT("Com_Transform"));
	_vector vLookPos = pTran->Get_State(CTransform::STATE_POSITION);


	m_pTransformCom->MoveTarget_Lend(vLookPos, 1.f, fTimeDelta, m_pNavigationCom, 1.f);


	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::Tick_Attacked(_float fTimeDelta)
{
}

void CMonster::Tick_Die(_float fTimeDelta)
{
}




void CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom);

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMonster::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{	
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}	


	Render_Col();


	return S_OK;
}

void CMonster::OnCollision(CGameObject * pOther)
{

	int i = 0;
}














HRESULT CMonster::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
		
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Mad_Crow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 3806;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	return S_OK;
}










CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}