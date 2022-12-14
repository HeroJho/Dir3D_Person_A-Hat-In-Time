#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CStaticModel_Col final : public CGameObject
{
public:
	typedef struct tagStaticModelDesc
	{
		TCHAR		cModelTag[MAX_PATH];
		_float3		vPos;
		_float3		vAngle;
		_float3		vScale;

		_float3 vCenter;
		_float3 vRotation;
		_float3 vSize;
		_bool	bWall;
		_int	iTagID;

		_int iNaviIndexSize;
		_int*	piNaviIndexs;


	}STATICMODELDESC;

private:
	CStaticModel_Col(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticModel_Col(const CStaticModel_Col& rhs);
	virtual ~CStaticModel_Col() = default;



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	
private:
	TCHAR					m_cModelTag[MAX_PATH];
	_float3					m_vAxis;

private:
	HRESULT Ready_Components(STATICMODELDESC* Desc);

public:
	static CStaticModel_Col* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END