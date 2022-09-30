#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "Player.h"

BEGIN(Engine)

class CGameObject;

END


BEGIN(Client)

class CAnimManager final : public CBase
{
public:
	enum EDIT_TYPE { EDIT_MODEL, EDIT_PLAYER, EDIT_END };
	DECLARE_SINGLETON(CAnimManager)
public:
	CAnimManager();
	virtual ~CAnimManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);

	// For. Load Model
public:
	string Get_PickedString() { return m_sPickedString; }
	void Set_PickedString(string sPickedString) { m_sPickedString = sPickedString; }

	class CAnimModel* Get_AnimModel() { return m_pAnimModel; }
	class CPlayer*	Get_PlayerModel() { return m_pPlayer; }


	_bool Get_IsDelete() { return m_bIsDelete; }
	void  Set_IsDelete(_bool bIsDelete) { m_bIsDelete = bIsDelete; }

public:

	HRESULT Create_Try_BinModel(const _tchar* pModelName);

	void Create_Model();
	void Delete_Model();

	void Create_Player();
	void Delete_Player();

	void Conv_Bin_Anim();

private:
	_tchar*     m_cLoadingChar = nullptr;

	_bool		m_bIsDelete = false;







	// For. Edit Anim
public:
	_int Get_AnimCount(EDIT_TYPE eType);
	_int Get_CurAimIndex(EDIT_TYPE eType);
	void Change_Anim(EDIT_TYPE eType, _int iIndex);
	void Delete_Anim(EDIT_TYPE eType, _int iIndex);

	_float Get_AnimSpeed(EDIT_TYPE eType);
	void Set_AnimSpeed(EDIT_TYPE eType, _float fSpeed);

	int* Get_KeyA() { return &m_iA; }
	int* Get_KeyD() { return &m_iD; }
	int* Get_KeyNone() { return &m_iNone; }
	_bool* Get_KeyTest() { return &m_bKeyTest; }

private:
	_int Get_AnimCount(CGameObject* pObj);
	_int Get_CurAimIndex(CGameObject* pObj);
	void Change_Anim(CGameObject* pObj, _int iIndex);
	void Delete_Anim(CGameObject* pObj, _int iIndex);
	_float Get_AnimSpeed(CGameObject* pObj);
	void Set_AnimSpeed(CGameObject* pObj, _float fSpeed);

	//  For. Player
public:
	_float Get_Player_AnimSpeed(CPlayer::STATE eType);
	void Set_Player_AnimSpeed(CPlayer::STATE eType, _float fSpeed);

	void Set_AnimLinearData();
	_int* Get_iMyIndex() { return &m_iMyIndex; }
	_int* Get_iTargetIndex() { return &m_iTargetIndex; }
	_float* Get_fTickperSceconed() { return &m_fTickperSceconed; }
	_float* Get_fLimitRatio() { return &m_fLimitRatio; }

	list<ANIM_LINEAR_DATA>* Get_LinearLists() { return &m_LinearLists; }

private:

	int	m_iA = 0;
	int	m_iD = 0;
	int	m_iNone = 0;
	_bool m_bKeyTest = false;


	list<ANIM_LINEAR_DATA> m_LinearLists;
	_int m_iMyIndex = 0;
	_int m_iTargetIndex = 0;
	_float m_fTickperSceconed = 0.f;
	_float m_fLimitRatio = 0.f;


	// For. Static
public:
	_bool* Get_PlayMode() { return &m_bPlayMode; }
	char* Get_CurAnimName(EDIT_TYPE eType);

private:
	class CAnimModel*	m_pAnimModel = nullptr;
	class CPlayer*		m_pPlayer = nullptr;
	string				m_sPickedString;

	_bool				m_bPlayMode = false;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;

};

END
