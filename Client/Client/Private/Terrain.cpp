#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (nullptr == pArg)
	{
		m_eLevel = LEVEL_GAMEPLAY;
	}
	else
	{
		TERRAINDESCs* pDesc = (TERRAINDESCs*)pArg;
		m_eLevel = pDesc->eLevel;
	}
	


	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (FAILED(Ready_FilterTexture()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 9999.f, 0.f, 1.f));

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	// m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_P))
	{
		if (m_bTest)
			m_bTest = false;
		else
			m_bTest = true;
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CTerrain::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	if(m_bTest)
		m_pNavigation->Render();
#endif

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
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

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Map_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.m_pNavigation */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigation)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	//const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (LIGHTDESC::TYPE_DIRECTIONAL == pLightDesc->eType)
	//{
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//		return E_FAIL;
	//}

	//else
	//{
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
	//		return E_FAIL;
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
	//		return E_FAIL;

	//}

	//

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Ready_FilterTexture()
{
	ID3D11Texture2D*		pTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pFilterSRV)))
		return E_FAIL;

	_ulong*		pPixel = new _ulong[256 * 256];
	ZeroMemory(pPixel, sizeof(_ulong) * (256 * 256));

	for (_uint i = 0; i < 256; ++i)
	{
		for (_uint j = 0; j < 256; ++j)
		{
			_uint iIndex = i * 256 + j;

			if(j < 128)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0,0);
		}
	}

	
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pPixel, sizeof(_ulong) * 256 * 256);

	m_pContext->Unmap(pTexture, 0);

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture, TEXT("../Bin/Filter.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixel);

	Safe_Release(pTexture);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pTextureCom : m_pTextureCom)	
		Safe_Release(pTextureCom);	
	
	Safe_Release(m_pNavigation);
	Safe_Release(m_pFilterSRV);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
