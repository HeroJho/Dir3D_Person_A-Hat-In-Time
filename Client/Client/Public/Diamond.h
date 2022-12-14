#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item.h"

#include "ItemManager.h"

BEGIN(Engine)
class CModel;
END



BEGIN(Client)

class CDiamond final : public CItem
{
private:
	CDiamond(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDiamond(const CDiamond& rhs);
	virtual ~CDiamond() = default;

private:
	CNavigation* m_pNavigationCom = nullptr;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix) override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	virtual void Use_Item() override;

	void Get_Tick(_float fTimeDelta);

public:
	void Init_Pigic_Bounding(_float OriJumpPow, _float OriDirPow);
	void Tick_Pigic_Bounding(_float fTimeDelta);


private:
	CModel*					m_pModelCom2 = nullptr;

private:
	CItemManager::SPRINTDESC m_Desc;

	_bool m_bStop = false;

	_bool m_bOn = false;
	_float m_fJumpAcc = 0.f;
	_float m_fJumpPower = 0.f;
	_float m_fDirPower = 0.f;


	_bool m_bGet = false;
	_bool m_bTimeGet = false;

	_float m_fRatio = 0.f;

	_float m_fLifeTimeAcc = 0.f;

protected:
	HRESULT Ready_Components();


public:
	static CDiamond* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END