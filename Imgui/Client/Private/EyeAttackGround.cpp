#include "stdafx.h"
#include "..\Public\EyeAttackGround.h"
#include "GameInstance.h"

#include "MapManager.h"
#include "MeshManager.h"
#include "ToolManager.h"


CEyeAttackGround::CEyeAttackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CEyeAttackGround::CEyeAttackGround(const CEyeAttackGround & rhs)
	: CGameObject(rhs)
{

}




HRESULT CEyeAttackGround::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CEyeAttackGround::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	m_vOriScale = _float3(1.f, 1.f, 1.f);

	return S_OK;
}








void CEyeAttackGround::Tick(_float fTimeDelta)
{

	_float3 vScale = m_pTransformCom->Get_Scale();
	vScale.x -= fTimeDelta * m_fScaleSpeed;

	if (0.7f > vScale.x)
	{
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriScale));
		m_fScaleSpeed += 0.1f;
	}
	else
	{
		m_pTransformCom->Set_Scale(XMVectorSet(vScale.x, vScale.y, vScale.x, 1.f));
	}

	if (4.f < m_fScaleSpeed)
		m_b = true;

	 // m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 6.f, fTimeDelta);
	
}









void CEyeAttackGround::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	if(!m_b)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}







HRESULT CEyeAttackGround::Render()
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



	
	if (FAILED(m_pShaderCom->Set_RawValue("g_Speed", &m_fScaleSpeed, sizeof(_float))))
		return E_FAIL;
	


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 5)))
			return E_FAIL;
	}



	return S_OK;
}







HRESULT CEyeAttackGround::Ready_Components()
{

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return S_OK;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("EyeAttackGround"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_split_line"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom0)))
	//	return E_FAIL;
	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ColorNoise"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom1)))
	//	return E_FAIL;

	return S_OK;
}






CEyeAttackGround * CEyeAttackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEyeAttackGround*		pInstance = new CEyeAttackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEyeAttackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEyeAttackGround::Clone(void * pArg)
{
	CEyeAttackGround*		pInstance = new CEyeAttackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEyeAttackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEyeAttackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom0);
	Safe_Release(m_pTextureCom1);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

void CEyeAttackGround::Tick_ImGui(_float fTimeDelta)
{
	ImGui::Begin("CEyeAttackGround");


	if (ImGui::Button("Replay"))
	{
		m_b = false;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriScale));
		m_fScaleSpeed = 0.1f;
	}



	ImGui::InputFloat("ScaleSpeed", &m_fScaleSpeed);

	_float3 vScale = m_pTransformCom->Get_Scale();
	if (ImGui::DragFloat3("Scale", (float*)&vScale))
		m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&vScale), 1.f));

	ImGui::InputFloat("RotSpeed", &m_fRatio);



	ImGui::End();
}
