#include "stdafx.h"
#include "..\Public\UI_Item_Inven.h"
#include "GameInstance.h"

#include "ItemManager.h"

#include "UI_Item_Inven_Slot.h"



CUI_Item_Inven::CUI_Item_Inven(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Item_Inven::CUI_Item_Inven(const CUI_Item_Inven & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Item_Inven::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Item_Inven::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;




	CUI_Item_Inven_Slot::ITEMBUTTONDESC IconDesc;
	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_SprintYarn"));
	Make_ChildUI(100.f, -11.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);

	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_WitchYarn"));
	Make_ChildUI(0.f, -11.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);

	lstrcpy(IconDesc.pIconTag, TEXT("Prototype_Component_Texture_Icon_IceYarn"));
	Make_ChildUI(-100.f, -11.f, 256.f / 4.f, 256.f / 4.f, TEXT("Prototype_UI_Item_Inven_Slot"), &IconDesc);



	m_vOnPos.x = m_UiInfo.fX;
	m_vOnPos.y = m_UiInfo.fY;

	m_vOffPos.x = m_UiInfo.fX;
	m_vOffPos.y = m_UiInfo.fY - 120.f;

	m_bIsOn = false;
	m_UiInfo.fY = m_vOffPos.y;



	return S_OK;
}

void CUI_Item_Inven::Tick(_float fTimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_I))
	{
		if (m_bIsOn)
			m_bIsOn = false;
		else
			m_bIsOn = true;
	}


	if (!m_bIsOn)
	{
		m_UiInfo.fY -= 300.f * fTimeDelta;

		if (m_UiInfo.fY < m_vOffPos.y)
			m_UiInfo.fY = m_vOffPos.y;

	}
	else
	{
		m_UiInfo.fY += 300.f * fTimeDelta;

		if (m_UiInfo.fY > m_vOnPos.y)
			m_UiInfo.fY = m_vOnPos.y;
	}

	if (m_bIsOn)
	{
		m_fTimeAcc += fTimeDelta;

		if (10.f < m_fTimeAcc)
		{
			m_bIsOn = false;
			m_fTimeAcc = 0.f;
		}
	}
	else
	{
		m_fTimeAcc = 0.f;
	}

	if (!m_bIsOn)
	{
		m_UiInfo.fY -= 600.f * fTimeDelta;

		if (m_UiInfo.fY < m_vOffPos.y)
			m_UiInfo.fY = m_vOffPos.y;
	}
	else
	{
		m_UiInfo.fY += 600.f * fTimeDelta;

		if (m_UiInfo.fY > m_vOnPos.y)
			m_UiInfo.fY = m_vOnPos.y;
	}




	RELEASE_INSTANCE(CGameInstance);

	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CUI_Item_Inven::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Item_Inven::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();

	// ???????? Render ????
	__super::Render();


	return S_OK;
}

void CUI_Item_Inven::LoadItemMgr_ItemUI()
{

	list<CItemManager::ITEMINFODESC>* items = CItemManager::Get_Instance()->Get_Items();


	for (auto& pChild : m_pChildUIs)
	{
		CUI_Item_Inven_Slot* pSlot = (CUI_Item_Inven_Slot*)pChild;

		for (auto& pItem : *items)
		{
			if (!lstrcmp(pSlot->Get_ItemModelTag(), pItem.szModelName))
			{
				pSlot->Set_Count(pItem.iCount);
				break;
			}
		}
	}

}


HRESULT CUI_Item_Inven::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopWindow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Item_Inven * CUI_Item_Inven::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Item_Inven*		pInstance = new CUI_Item_Inven(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Item_Inven::Clone(void * pArg)
{
	CUI_Item_Inven*		pInstance = new CUI_Item_Inven(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Edit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Item_Inven::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
