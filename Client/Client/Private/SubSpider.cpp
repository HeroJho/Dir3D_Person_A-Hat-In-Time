#include "stdafx.h"
#include "..\Public\SubSpider.h"
#include "GameInstance.h"
#include  "ToolManager.h"

#include "Player.h"

CSubSpider::CSubSpider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSubSpider::CSubSpider(const CSubSpider & rhs)
	: CMonster(rhs)
{
}






HRESULT CSubSpider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubSpider::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_CreatureDesc.vPos), 1.f));
	m_pTransformCom->Set_CurSpeed(1.f);

	Set_State(SPIDER_UPIDLE);

	m_pTransformCom->Set_RealOriScale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	XMStoreFloat3(&m_vDownPos,	m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat3(&m_vUpPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 10.f, 0.f, 0.f));


	return S_OK;
}







void CSubSpider::Set_AnimLinearData(ANIM_LINEAR_DATA LinearData)
{
	m_pModelCom->Push_AnimLinearData(LinearData);
}

void CSubSpider::Set_State(SPIDER_STATE eState)
{
	m_ePreState = m_eState;
	m_eState = eState;


	if (m_ePreState != m_eState)
	{
		/*switch (m_eState)
		{
		case Client::CSubSpider::MONSTER_ATTACKED:
			break;
		}*/
	}

	Set_Anim();
}

void CSubSpider::Set_Anim()
{

	switch (m_eState)
	{
	case Client::CSubSpider::SPIDER_UPIDLE:
		m_pModelCom->Set_AnimIndex(3);
		break;
	case Client::CSubSpider::SPIDER_DOWNIDLE:
		m_pModelCom->Set_AnimIndex(1);
		break;
	case Client::CSubSpider::SPIDER_DOWN:
		m_pModelCom->Set_AnimIndex(0);
		break;
	case Client::CSubSpider::SPIDER_UP:
		m_pModelCom->Set_AnimIndex(4);
		break;
	case Client::CSubSpider::SPIDER_ATTACK:
		m_pModelCom->Set_AnimIndex(2);
		break;
	case Client::CSubSpider::SPIDER_ATTACKED:
		break;
	case Client::CSubSpider::SPIDER_DIE:
		break;
	}

}



void CSubSpider::Attacked(_int iAT)
{
	--m_CreatureDesc.iHP;
	if (0 >= m_CreatureDesc.iHP)
	{
		Set_State(SPIDER_DIE);
	}
	else
	{
		Set_State(SPIDER_ATTACKED);
		m_pTransformCom->ReSet_AttackedAnim();
	}

}






void CSubSpider::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	switch (m_eState)
	{
	case SPIDER_UPIDLE:
		UpIdle_Tick(fTimeDelta);
		break;
	case SPIDER_DOWNIDLE:
		DownIdle_Tick(fTimeDelta);
		break;
	case SPIDER_DOWN:
		Down_Tick(fTimeDelta);
		break;
	case SPIDER_UP:
		Up_Tick(fTimeDelta);
		break;
	case SPIDER_ATTACK:
		Attack_Tick(fTimeDelta);
		break;
	case SPIDER_ATTACKED:
		Attacked_Tick(fTimeDelta);
		break;
	case SPIDER_DIE:
		Die_Tick(fTimeDelta);
		break;
	}

}

void CSubSpider::UpIdle_Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vUpPos), 1.f));
}

void CSubSpider::DownIdle_Tick(_float fTimeDelta)
{
	if (!m_bOn)
	{
		m_fDownIdleTimeAcc += fTimeDelta;
		if (1.f < m_fDownIdleTimeAcc)
		{
			Set_State(SPIDER_UP);
			m_fDownIdleTimeAcc = 0.f;
		}
	}
	else
		m_fDownIdleTimeAcc = 0.f;
	

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vDownPos), 1.f));
}

void CSubSpider::Down_Tick(_float fTimeDelta)
{
	_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vDownPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (0.1f < fDis)
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 5.f, fTimeDelta);
	else
		Set_State(SPIDER_DOWNIDLE);
}

void CSubSpider::Up_Tick(_float fTimeDelta)
{
	_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vUpPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (0.1f < fDis)
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 5.f, fTimeDelta);
	else
		Set_State(SPIDER_UPIDLE);
}

void CSubSpider::Attack_Tick(_float fTimeDelta)
{
}

void CSubSpider::Attacked_Tick(_float fTimeDelta)
{
	if (m_pTransformCom->Tick_AttackAnim(fTimeDelta))
		Set_State(m_ePreState);

}

void CSubSpider::Die_Tick(_float fTimeDelta)
{
	m_fDeadTimeAcc += fTimeDelta;
	if (30.f < m_fDeadTimeAcc)
	{
		Set_State(SPIDER_UPIDLE);
		m_fDeadTimeAcc = 0.f;
	}
}


void CSubSpider::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_MONSTER);

	if (nullptr == m_pRendererCom)
		return;




	_matrix mWorld = m_pTransformCom->Get_OriScaleWorldMatrix();
	mWorld.r[3] = XMVectorSetW(XMLoadFloat3(&m_vDownPos), 1.f);
	Tick_Col(mWorld);



	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		if (SPIDER_ATTACK == m_eState)
			Set_State(SPIDER_DOWNIDLE);
	}



	// ������ ��� ����.
	if (SPIDER_DIE != m_eState)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);


		_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);
		RELEASE_INSTANCE(CGameInstance);

		if (true == isDraw)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
	}



	m_bOn = false;
}

HRESULT CSubSpider::Render()
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
		if (SPIDER_ATTACK != m_eState && 1 == i)
			continue;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}


	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();


	return S_OK;
}

void CSubSpider::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("ChaseSphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
		{
			if (SPIDER_UPIDLE == m_eState)
				Set_State(SPIDER_DOWN);

			m_bOn = true;
		}

		if (!strcmp("Attacked_Sphere", Desc.MyDesc.sTag) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag)
			&& SPIDER_ATTACKED !=  m_eState)
		{
			CPlayer* pPlayer = (CPlayer*)Desc.pOther;

			if (CPlayer::STATE_JUMPATTACK != pPlayer->Get_State() &&
				CPlayer::STATE_JUMPATTACKRENDING != pPlayer->Get_State() &&
				false == pPlayer->Get_Attacked())
			{
				Set_State(SPIDER_ATTACK);
				pPlayer->Attacked();
			}
		}
	}
}














HRESULT CSubSpider::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SubSpider"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(.5f, .5f, .5f);
	strcpy(ColDesc.sTag, "Attacked_Sphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(6.f, 6.f, 6.f);
	strcpy(ColDesc.sTag, "ChaseSphere");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}












CSubSpider * CSubSpider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSubSpider*		pInstance = new CSubSpider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubSpider::Clone(void * pArg)
{
	CSubSpider*		pInstance = new CSubSpider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSubSpider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubSpider::Free()
{
	__super::Free();


}