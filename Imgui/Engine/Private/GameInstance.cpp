#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())

{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
}


HRESULT CGameInstance::Initialize_Engine(_uint iNumLevel, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	/* 엔진을 구동하기위한 초기화작업을 거치낟. */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 그래픽디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDeviceOut, ppDeviceContextOut)))
		return E_FAIL;

	/* 입력 초기화. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드 초기화. */

	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, *ppDeviceOut, *ppDeviceContextOut)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevel)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevel)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPicking->Tick();

	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}


HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}



HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, CGameObject ** pObj, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pObj, pArg);
}

HRESULT CGameInstance::Check_Prototype(const _tchar * pPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Check_Prototype(pPrototypeTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Check_Prototype(_int iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Check_Prototype(iLevelIndex, pPrototypeTag);
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Update_Timer(pTimerTag);
}





const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);
}






_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_bool CGameInstance::Key_Down(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Down(eKeyID);
}

_bool CGameInstance::Key_Up(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Up(eKeyID);
}

_bool CGameInstance::Key_Pressing(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Pressing(eKeyID);
}

_bool CGameInstance::Mouse_Down(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Down(eMouseKeyID);
}

_bool CGameInstance::Mouse_Up(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Up(eMouseKeyID);
}

_bool CGameInstance::Mouse_Pressing(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Pressing(eMouseKeyID);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();
	
	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}



void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();
	
	CLight_Manager::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();	
	
	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	
}
