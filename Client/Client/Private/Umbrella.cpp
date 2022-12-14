#include "stdafx.h"
#include "..\Public\Umbrella.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "ParticleManager.h"

#include "Player.h"
#include "Monster.h"
#include "VSnatcher.h"


CUmbrella::CUmbrella(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CUmbrella::CUmbrella(const CUmbrella & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CUmbrella::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUmbrella::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CSockat::PARTSDESC* Desc = (CSockat::PARTSDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Umbrella";

	if (nullptr != Desc->pOwner)
	{
		m_pOwner = Desc->pOwner;
		//Safe_AddRef(m_pOwner);
	}


	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vRot.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vRot.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vRot.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);




	return S_OK;
}

void CUmbrella::Tick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	CPlayer* pPlayer = ((CPlayer*)m_pOwner);
	CPlayer::STATE ePlayerCurState = pPlayer->Get_State();

	if (CPlayer::STATE_READYATTACK == ePlayerCurState)
	{
		m_fAttackTimeAcc = 0.f;
		m_bCanAttack = false;
	}

	if (CPlayer::STATE_ATTACK_1 == ePlayerCurState ||
		CPlayer::STATE_ATTACK_2 == ePlayerCurState ||
		CPlayer::STATE_ATTACK_3 == ePlayerCurState )
	{
		m_fAttackTimeAcc += fTimeDelta;
	}
	else
	{
		m_fAttackTimeAcc = 0.f;
		m_bCanAttack = false;
	}


	if (0.05f < m_fAttackTimeAcc && 0.2f > m_fAttackTimeAcc)
	{
		m_bCanAttack = true;
		m_fAttackTimeAcc = -1.f;
	}

}

void CUmbrella::LateTick(_float fTimeDelta)
{
	fTimeDelta *= CToolManager::Get_Instance()->Get_TimeRatio(CToolManager::TIME_EM);

	if (nullptr == m_pRendererCom)
		return;

	// ?????? ???? ???? ?????????? ???????? ???????? ??????.
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	Tick_Col(WorldMatrix);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Add_ColGroup(CColliderManager::COLLIDER_SWORD, this);
	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CUmbrella::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
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

	if (CToolManager::Get_Instance()->Get_Debug())
		Render_Col();

	return S_OK;
}

HRESULT CUmbrella::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CUmbrella::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (!m_bCanAttack)
		return;

	if (("Tag_Monster" == Desc.pOther->Get_Tag() || "BageS_Base" == Desc.pOther->Get_Tag() || "Tag_Snatcher" == Desc.pOther->Get_Tag()) && !strcmp("Attacked_Sphere", Desc.OtherDesc.sTag))
	{
		// ???? ????
		if ("Tag_Snatcher" == Desc.pOther->Get_Tag())
		{
			((CVSnatcher*)Desc.pOther)->Attacked();
			m_bCanAttack = false;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pParentTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.3f, 0.3f, 0.3f), _float3(0.4f, 0.4f, 0.4f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 3.f, false, 0.f, 0.f, 1.5f,
				5, 1.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);

		}
		else
		{
			((CMonster*)Desc.pOther)->Attacked(1);
			m_bCanAttack = false;

			_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pParentTransformCom->Get_State(CTransform::STATE_POSITION));
			CParticleManager::Get_Instance()->Create_Effect(TEXT("Prototype_Component_Texture_Star"), vPos, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.3f, 0.3f, 0.3f), _float3(0.4f, 0.4f, 0.4f), _float3(0.f, 0.f, 0.f), _float3(-90.f, 0.f, 0.f), 0.1f, 3.f, false, 0.f, 0.f, 1.f,
				5, 1.f, 0.1f, 0.f, 0.f, 0.f, 0.2f, 0.f, 0.0f, 0.2f, _float3(0.f, 0.f, 0.f), _float3(360.f, 0.f, 360.f), CParticle::TYPE_TEXTURE);

		}
	}
}

HRESULT CUmbrella::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Umbrella"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 0.4f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.8f, 0.5f);
	ColDesc.bIsStatic = false;
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_AABB, ColDesc)))
	//	return E_FAIL;

	//ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;

	return S_OK;
}

CUmbrella * CUmbrella::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUmbrella*		pInstance = new CUmbrella(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUmbrella"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUmbrella::Clone(void * pArg)
{
	CUmbrella*		pInstance = new CUmbrella(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUmbrella"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUmbrella::Free()
{
	__super::Free();


	// Safe_Release(m_pOwner);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
