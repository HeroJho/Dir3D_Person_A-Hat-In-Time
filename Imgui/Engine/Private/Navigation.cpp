#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_NavigationDesc(rhs.m_NavigationDesc)
	, m_pShader(rhs.m_pShader)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

	Safe_AddRef(m_pShader);
}

HRESULT CNavigation::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	for (_uint i = 0; i < 10; ++i)
	{
		for (_uint j = 0; j < 10; ++j)
		{

			_float3			vPoints[3];
			vPoints[0].x = j;
			vPoints[0].y = 0;
			vPoints[0].z = i + 1;

			vPoints[1].x = j + 1;
			vPoints[1].y = 0;
			vPoints[1].z = i;

			vPoints[2].x = j;
			vPoints[2].y = 0;
			vPoints[2].z = i;

			CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
			if (nullptr == pCell)
				return E_FAIL;

			m_Cells.push_back(pCell);


			vPoints[0].x = j;
			vPoints[0].y = 0;
			vPoints[0].z = i + 1;

			vPoints[1].x = j + 1;
			vPoints[1].y = 0;
			vPoints[1].z = i + 1;

			vPoints[2].x = j + 1;
			vPoints[2].y = 0;
			vPoints[2].z = i;

			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
			if (nullptr == pCell)
				return E_FAIL;

			m_Cells.push_back(pCell);

		}
	}


	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{

	if (nullptr != pArg)
		memcpy(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}


_float CNavigation::Compute_Height(_fvector vPos)
{
	CCell* pCell = m_Cells[m_NavigationDesc.iCurrentIndex];

	_vector vA = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_A)), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_B)), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&pCell->Get_Point(CCell::POINT_C)), 1.f);

	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);


	_vector fTargetPos = vPos;
	_float fx = XMVectorGetX(fTargetPos);
	_float fz = XMVectorGetZ(fTargetPos);

	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	/* y = (-ax - cz - d) / b */

	return (-fa * fx - fc * fz - fd) / fb;
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int	iNeighborIndex = -1;

	/* ���� �� �ȿ��� ��������. */
	/* �������⿡ �̿��� �ִٸ�. �̿��� �Τ��Խ��� �޾ƿ���.
	�̿��� ���ٸ� ��ä���´�. */
	if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
		return true;

	/* ���� ���� ������. */
	else
	{
		/* �������⿡ �̿��� �־��ٸ�. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (0 > iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
				{
					/* Ŀ��Ʈ �ε����� �̿��� �ε����� �ٲ��ش�. */
					m_NavigationDesc.iCurrentIndex = iNeighborIndex;

					return true;
				}
			}

		}

		/* �������⿡ �̿��� �����ٸ�. */
		else
			return false;


	}


	return _bool();
}

_bool CNavigation::isGround(_fvector vPosition, _float* OutfCellY)
{
	_float fCellY = Compute_Height(vPosition);
	_float fY = XMVectorGetY(vPosition);

	*OutfCellY = fCellY;

	if (fCellY >= fY)
		return true;

	return false;
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{

	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	/*WorldMatrix.m[1][3] = fHeight;*/

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render_Cell();
	}

	return S_OK;
}

#endif // _DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				// ��(Dest)�� ��(Sour) AB�̿�. 
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell);
			}
		}
	}


	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNavigation*			pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*			pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	Safe_Release(m_pShader);


	m_Cells.clear();
}