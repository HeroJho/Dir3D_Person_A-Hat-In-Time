#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Map_Terrain;
class CNavigation;
END

BEGIN(Client)

class CTerrain_Anim : public CGameObject
{
protected:
	CTerrain_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain_Anim(const CTerrain_Anim& rhs);
	virtual ~CTerrain_Anim() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Map_Terrain*	m_pVIBufferCom = nullptr;
	CNavigation*			m_pNavigation = nullptr;

private:
	_float3					m_vMousePickPos;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값 던진다. */

public:
	static CTerrain_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END