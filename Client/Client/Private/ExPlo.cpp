#include "stdafx.h"
#include "..\Public\ExPlo.h"
#include "GameInstance.h"

#include "IceBox.h"
#include "Player.h"

CExPlo::CExPlo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CExPlo::CExPlo(const CExPlo& rhs)
	: CGameObject(rhs)
{
}

HRESULT CExPlo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExPlo::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;




	if (FAILED(Ready_Components()))
		return E_FAIL;

	EXPLODESC* Desc = (EXPLODESC*)pArg;



	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CExPlo::Tick(_float fTimeDelta)
{
	if (m_bTickAttack)
		Set_Dead(true);
}

void CExPlo::LateTick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_PROJECT, this);
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	m_bTickAttack = true;
}

HRESULT CExPlo::Render()
{


	Render_Col();

	return S_OK;
}



HRESULT CExPlo::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, , TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	strcpy(ColDesc.sTag, "EX_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}



void CExPlo::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!strcmp("EX_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		if ("Tag_Player" == Desc.pOther->Get_Tag())
		{
			CPlayer* pPlayer = (CPlayer*)Desc.pOther;
			pPlayer->Attacked();
		}

		m_bTickAttack = true;
	}
}



CExPlo * CExPlo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExPlo*		pInstance = new CExPlo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExPlo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExPlo::Clone(void * pArg)
{
	CExPlo*		pInstance = new CExPlo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExPlo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CExPlo::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}