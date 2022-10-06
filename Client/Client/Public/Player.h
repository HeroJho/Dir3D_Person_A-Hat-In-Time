#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CSockat;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_SPRINT, STATE_SLEP, STATE_ATTACK_1, STATE_ATTACK_2, STATE_ATTACK_3, STATE_READYATTACK, STATE_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pOther) override;

public:
	void Set_AnimLinearData(ANIM_LINEAR_DATA LinearData);
	CGameObject* Add_Sockat(char* pBoneName, _tchar* cName);


private:
	void Set_State();
	void Set_Anim();
	void Check_EndAnim();

	void Idle_Tick(_float fTimeDelta);
	void Move_Tick(_float fTimeDelta);
	void Slep_Tick(_float fTimeDelta);

	void Move_Input(_float fTimeDelta);
	void Attack_Input(_float fTimeDelta);
	void ReadyAttack_Input(_float fTimeDelta);

	void Calcul_State(_float fTimeDelta);


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSockat*				m_pSockatCom = nullptr;

private:
	_float3					m_vAxis;
	STATE				m_eState = STATE_END;
	STATE				m_ePreState = STATE_END;
	list<STATE>			m_TickStates;
	list<STATE>			m_ComboStates;


	_float3				m_vDestLook;
	_float				m_fCulSpeed = 0.f;
	_float				m_fWalkSpeed = 0.f;
	_float				m_fRunSpeed = 0.f;
	_float				m_fTurnSpeed = 0.f;
	_float				m_fRotationSpeed = 0.f;
	_float				m_fSlepSpeed = 0.f;

	_bool				m_bImStop = false;

private:
	HRESULT Ready_Components();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END