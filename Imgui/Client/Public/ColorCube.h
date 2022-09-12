#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_ColorCube;
END

BEGIN(Client)

class CColorCube final : public CGameObject
{
public:
	typedef struct tagColorCubeDesc
	{
		_uint iR;
		_uint iG;
		_uint iB;
		_float3 vPos;
		_float3 vScale;
	}COLORCUBEDESC;

private:
	CColorCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColorCube(const CColorCube& rhs);
	virtual ~CColorCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Pos(_float3 vPos);
	_float3 Get_Pos();

	void Set_LinkIndex(_int iIndex) { m_iLinkIndex = iIndex; }
	_int Get_LinkIndex() { return m_iLinkIndex; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_ColorCube*	m_pVIBufferCom = nullptr;

private:
	_float3			m_vRGB;
	_int m_iLinkIndex = 0;

private:
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
private:
	HRESULT SetUp_Components();

public:
	static CColorCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END