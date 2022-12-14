#include "stdafx.h"
#include "..\Public\Hat.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "Player.h"

CHat::CHat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CItem(pDevice, pContext)
{

}

CHat::CHat(const CHat & rhs)
	: CItem(rhs)
{

}

HRESULT CHat::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CHat::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_sTag = "Tag_Item";




	ITEMDESC* ItemDesc = (ITEMDESC*)pArg;
	if (nullptr == ItemDesc->pDesc)
	{
		if (FAILED(Ready_Components()))
			return E_FAIL;
	}
	else
	{
		CItemManager::SPRINTDESC* YarnDesc = (CItemManager::SPRINTDESC*)(ItemDesc->pDesc);
		memcpy(&m_Desc, YarnDesc, sizeof(CItemManager::SPRINTDESC));
		m_Desc.bIsPigic = true;

		if (FAILED(Ready_Components()))
			return E_FAIL;

		Init_Pigic_Bounding(m_Desc.fJumpPow, m_Desc.fDirPow);
	}


	return S_OK;
}

void CHat::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	__super::Tick(fTimeDelta);


	Tick_Pigic_Bounding(fTimeDelta);

}

void CHat::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);


	if (nullptr == m_pNavigationCom)
	{
		_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
		Tick_Col(mWorld, nullptr, nullptr);
	}
	else
	{
		_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();

		if (m_bStop && m_bOn)
			Tick_Col(mWorld, nullptr, nullptr);
		else
		{
			Tick_Col(m_pTransformCom->Get_WorldMatrix(), m_pNavigationCom, m_pTransformCom, 0.25f);
			m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 3.f, 0.25f);
		}
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_ITEM, this);
	RELEASE_INSTANCE(CGameInstance);


	__super::LateTick(fTimeDelta);

}

HRESULT CHat::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CHat::SetUp_State(_fmatrix StateMatrix)
{
	if (FAILED(__super::SetUp_State(StateMatrix)))
		return E_FAIL;

	return S_OK;
}


void CHat::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if (!strcmp("Attacked_Sphere", Desc.OtherDesc.sTag) && lstrcmp(TEXT("crown") ,m_InvenDesc.szModelName))
		{
			if (m_Desc.bIsPigic)
			{
				if (Get_Dead())
					return;

				CPlayer* pPlayer = (CPlayer*)Desc.pOther;

				pPlayer->Get_Hat(m_InvenDesc.szModelName, true);

				Set_Dead(true);
			}
			else if (!m_Desc.bIsPigic)
			{
				if (Get_Dead())
					return;

				CPlayer* pPlayer = (CPlayer*)Desc.pOther;

				pPlayer->Get_Hat(m_InvenDesc.szModelName, true);

				Set_Dead(true);
			}

		}
	}
}






void CHat::Use_Item()
{



}

void CHat::Init_Pigic_Bounding(_float OriJumpPow, _float OriDirPow)
{
	m_fJumpPower = OriJumpPow;
	m_fDirPower = OriDirPow;
}

void CHat::Tick_Pigic_Bounding(_float fTimeDelta)
{

	if (nullptr == m_pNavigationCom || (m_bStop && m_bOn))
		return;

	if (m_bOn)
	{
		m_bOn = false;

		_float fJumpRatio = m_fJumpPower / m_Desc.fJumpPow;

		m_fJumpAcc += fTimeDelta * 4.f;
		m_fJumpPower -= m_fJumpAcc;

		m_fDirPower -= m_fJumpAcc;
		_float fDirRatio = m_fDirPower / m_Desc.fJumpPow;

		if (0.1f > fJumpRatio)
		{
			m_bStop = true;
		}
		else
		{
			m_pTransformCom->Jump(m_fJumpPower * fJumpRatio);
		}
	}


	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_Desc.vDir), m_fDirPower, fTimeDelta, m_pNavigationCom);


	// ???? ??????
	_float fTemp = 0.f;
	if (m_pNavigationCom->isGround(m_pTransformCom->Get_State(CTransform::STATE_POSITION), &fTemp, 0.25f)
		|| COBB::COL_ON == Get_StaticOBB()->Get_ColState())
	{
		m_bOn = true;
	}
	else if (COBB::COL_BLOCK == Get_StaticOBB()->Get_ColState())
	{
		m_pTransformCom->ResetGravity();
		m_fJumpPower = 0.f;
		m_fDirPower = 0.f;
	}

}






HRESULT CHat::Ready_Components()
{

	if (m_Desc.bIsPigic)
	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATIONDESC NaviDesc;
		ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
		NaviDesc.iCurrentIndex = m_Desc.iNaviIndex;
		if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	if (m_Desc.bIsPigic)
	{
		ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
		ColDesc.bIsStatic = true;
		strcpy(ColDesc.sTag, "Static_OBB");
		if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
			return E_FAIL;
	}

	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	strcpy(ColDesc.sTag, "Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}

CHat * CHat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHat*		pInstance = new CHat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHat::Clone(void * pArg)
{
	CHat*		pInstance = new CHat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat::Free()
{
	__super::Free();


	Safe_Release(m_pNavigationCom);

}


