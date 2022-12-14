#include "stdafx.h"
#include "..\Public\Terrain_Anim.h"
#include "GameInstance.h"

#include "MapManager.h"

CTerrain_Anim::CTerrain_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain_Anim::CTerrain_Anim(const CTerrain_Anim & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain_Anim::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Anim::Tick(_float fTimeDelta)
{
	// m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
}

void CTerrain_Anim::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain_Anim::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	ID3D11RasterizerState* m_WireFrame;
	D3D11_RASTERIZER_DESC temp;
	ZeroMemory(&temp, sizeof(D3D11_RASTERIZER_DESC));
	temp.FillMode = D3D11_FILL_WIREFRAME;
	temp.CullMode = D3D11_CULL_NONE;
	temp.DepthClipEnable = true;
	m_pDevice->CreateRasterizerState(&temp, &m_WireFrame);

	m_pContext->RSSetState(m_WireFrame);
	Safe_Release(m_WireFrame);

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pContext->RSSetState(nullptr);

	return S_OK;
}


HRESULT CTerrain_Anim::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Terrain"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CVIBuffer_Map_Terrain::TERRAINDESC TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof(CVIBuffer_Map_Terrain::TERRAINDESC));
	TerrainDesc.iNumVerticesX = 200;
	TerrainDesc.iNumVerticesY = 200;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &TerrainDesc)))
		return E_FAIL;


	/* For.m_pNavigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigation)))
		return E_FAIL;



	return S_OK;
}

HRESULT CTerrain_Anim::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_vMousePosition", &_float3(0.f, 0.f, 0.f), sizeof(_float3))))
		return E_FAIL;
	_float fRad = 5.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRad", &fRad, sizeof(_float))))
		return E_FAIL;
	_bool bColor = true;
	_float4 vColor(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_bColor", &bColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;


	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

CTerrain_Anim * CTerrain_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain_Anim*		pInstance = new CTerrain_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain_Anim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain_Anim::Clone(void * pArg)
{
	CTerrain_Anim*		pInstance = new CTerrain_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTerrain_Anim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pNavigation);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
