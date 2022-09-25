#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CStaticModel final : public CGameObject
{
public:
	typedef struct tagStaticModelDesc
	{
		TCHAR		cModelTag[MAX_PATH];
	}STATICMODELDESC;

private:
	CStaticModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;

public:
	_float3 Get_Axis() { return m_vAxis; }
	void Set_Axis(_float3 vAxis);

	void Set_ModelNum(string sModelNum) { m_sModelNum = sModelNum; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;


private:
	TCHAR					m_cModelTag[MAX_PATH];
	string					m_sModelNum;
	_float3					m_vAxis;

private:
	HRESULT Ready_Components();

public:
	static CStaticModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END