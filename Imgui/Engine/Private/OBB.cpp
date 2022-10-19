#include "..\Public\OBB.h"
#include "AABB.h"
#include "Sphere.h"

#include "Transform.h"
#include "Navigation.h"
#include "GameObject.h"

COBB::COBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

COBB::COBB(const COBB & rhs)
	: CCollider(rhs)
{

}

HRESULT COBB::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_OBB;

	return S_OK;
}

HRESULT COBB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_OBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f,
		m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians((m_ColliderDesc.vRotation.z)));

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOriginal_OBB->Center = m_ColliderDesc.vCenter;

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	return S_OK;
}




void COBB::Update(_fmatrix TransformMatrix, CNavigation* pNavi, CTransform* pTran)
{
	m_isColl = false;
	m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);

}

_bool COBB::Collision_OBB(CCollider * pTargetCollider, CTransform* pTran, _float3* Out_fPushDir, _float* Out_fPlanY)
{
	if (CCollider::TYPE_SPHERE == pTargetCollider->Get_ColliderType())
		return E_FAIL;

	m_isColl = false;
	((COBB*)pTargetCollider)->m_eColState = COL_NONE;

	OBBDESC			OBBDesc[2] = {
		Compute_OBBDesc(),
		((COBB*)pTargetCollider)->Compute_OBBDesc(),
	};

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_vector			vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);

			_float			fDistance[3] = { 0.0f };
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return m_isColl;

		}
	}


	m_isColl = true;

	return m_isColl;

}


_bool COBB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	bool bIsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		bIsColl = m_pOBB->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		bIsColl = m_pOBB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		bIsColl = m_pOBB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	if (!m_isColl)
		m_isColl = bIsColl;

	if (bIsColl)
		pTargetCollider->SetbCol();

	return bIsColl;
}

_bool COBB::Collision_Cell(_fvector vA, _fvector vB, _fvector vC, _fmatrix TransformMatrix)
{
	_vector vZeroA = XMVectorSetY(vA, 0.f);
	_vector vZeroB = XMVectorSetY(vB, 0.f);
	_vector vZeroC = XMVectorSetY(vC, 0.f);


	_matrix mZeroMatrix = TransformMatrix;
	mZeroMatrix.r[3] = XMVectorSetY(mZeroMatrix.r[3], 0.f);


	_float3 vCenterTemp = m_pOriginal_OBB->Center;

	m_pOriginal_OBB->Center = _float3(vCenterTemp.x, 0.f, vCenterTemp.z);
	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians(m_ColliderDesc.vRotation.z));

	RotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);



	m_pOriginal_OBB->Transform(*m_pOBB, mZeroMatrix);

	RotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);
	m_pOriginal_OBB->Center = vCenterTemp;


	return m_pOBB->Intersects(vZeroA, vZeroB, vZeroC);
}





COBB::OBBDESC COBB::Compute_OBBDesc()
{
	OBBDESC			OBBDesc;

	_float3			vPoints[8];

	m_pOBB->GetCorners(vPoints);

	XMStoreFloat3(&OBBDesc.vCenter, (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[4])) * 0.5f);

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[5])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[7])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[0])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[0])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[1])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[2])));

	return OBBDesc;
}

_float COBB::Compute_Height(_fvector vPos)
{
	_float3			vPoints[8];
	m_pOBB->GetCorners(vPoints);

	_vector vA = XMVectorSetW(XMLoadFloat3(&vPoints[2]), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&vPoints[7]), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&vPoints[3]), 1.f);


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

_float COBB::Compute_Height(_fvector vPos, _float3 * vPoss)
{
	_vector vA = XMVectorSetW(XMLoadFloat3(&vPoss[0]), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&vPoss[1]), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&vPoss[2]), 1.f);


	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);

	XMPlaneFromPoints(vA, vB, vC);

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

_bool COBB::Compute_LayPlane(_fvector vPos, _fvector vDir, _float3 * Out_pPoss)
{
	_float3			vPoints[8];
	m_pOBB->GetCorners(vPoints); // 나무의 OBB

	_float fMinDis = FLT_MAX;
	_float fDis = FLT_MAX;
	_bool bInter = false;

	_vector vVPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.5f);
	_vector vVDir = vDir;


	// 상
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[7]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[7]), fDis))
	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[2];
			Out_pPoss[1] = vPoints[7];
			Out_pPoss[2] = vPoints[3];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 하
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[1]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[4]), fDis))
	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[0];
			Out_pPoss[1] = vPoints[5];
			Out_pPoss[2] = vPoints[1];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 좌
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[0]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[4]), XMLoadFloat3(&vPoints[0]), fDis))

	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[3];
			Out_pPoss[1] = vPoints[7];
			Out_pPoss[2] = vPoints[0];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 우
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[2]), XMLoadFloat3(&vPoints[1]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[1]), XMLoadFloat3(&vPoints[5]), fDis))
	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[6];
			Out_pPoss[1] = vPoints[2];
			Out_pPoss[2] = vPoints[1];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 앞
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[5]), XMLoadFloat3(&vPoints[4]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[7]), XMLoadFloat3(&vPoints[6]), XMLoadFloat3(&vPoints[5]), fDis))
	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[7];
			Out_pPoss[1] = vPoints[5];
			Out_pPoss[2] = vPoints[4];
			fMinDis = fDis;
		}

		bInter = true;
	}

	// 뒤
	if (TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[1]), XMLoadFloat3(&vPoints[2]), fDis) ||
		TriangleTests::Intersects(vVPos, vVDir, XMLoadFloat3(&vPoints[3]), XMLoadFloat3(&vPoints[0]), XMLoadFloat3(&vPoints[1]), fDis))
	{
		if (fMinDis > fDis)
		{
			Out_pPoss[0] = vPoints[3];
			Out_pPoss[1] = vPoints[1];
			Out_pPoss[2] = vPoints[2];
			fMinDis = fDis;
		}

		bInter = true;
	}

	return bInter;
}

void COBB::Edit_Col(COLLIDERDESC Desc)
{
	_matrix		RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians((m_ColliderDesc.vRotation.z)));

	RotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	CCollider::Edit_Col(Desc);

	/* 회전에 대한 상태. */
	RotationMatrix = XMMatrixRotationX(XMConvertToRadians(m_ColliderDesc.vRotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(m_ColliderDesc.vRotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians((m_ColliderDesc.vRotation.z)));

	m_pOriginal_OBB->Extents = _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f),
	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);
	m_pOriginal_OBB->Center = m_ColliderDesc.vCenter;
}





HRESULT COBB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

COBB * COBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	COBB*			pInstance = new COBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * COBB::Clone(void * pArg)
{
	COBB*			pInstance = new COBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void COBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);

}
