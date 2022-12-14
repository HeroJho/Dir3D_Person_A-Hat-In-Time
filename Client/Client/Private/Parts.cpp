#include "stdafx.h"
#include "..\Public\Parts.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "WitchChargEffect.h"

CParts::CParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

CParts::CParts(const CParts & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_vAxis, sizeof(_float3));
}

HRESULT CParts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParts::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CSockat::PARTSDESC* Desc = (CSockat::PARTSDESC*)pArg;

	lstrcpy(m_cModelTag, Desc->m_szModelName);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&Desc->vScale));

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), Desc->vRot.x
		, XMVectorSet(0.f, 1.f, 0.f, 0.f), Desc->vRot.y
		, XMVectorSet(0.f, 0.f, 1.f, 0.f), Desc->vRot.z);

	_vector vPos = XMLoadFloat3(&Desc->vPos);
	vPos = XMVectorSetW(vPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_bBlur = Desc->bBlur;


	return S_OK;
}

void CParts::Tick(_float fTimeDelta)
{



}

void CParts::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	// m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CParts::Render()
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


	_bool bBlur = m_bBlur;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bBlur", &bBlur, sizeof(_bool))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	_uint iPassIndex = 0;
	if (!lstrcmp(TEXT("Sprint_Hat"), m_cModelTag))
		iPassIndex = 1;


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParts::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

_float3 CParts::Get_TotalPos()
{
	_matrix mTotal = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();

	_vector vPos = mTotal.r[3];
	_float3 vVPos;
	XMStoreFloat3(&vVPos, vPos);

	return vVPos;
}

HRESULT CParts::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_cModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CParts * CParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParts*		pInstance = new CParts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParts"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParts::Clone(void * pArg)
{
	CParts*		pInstance = new CParts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParts"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParts::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
