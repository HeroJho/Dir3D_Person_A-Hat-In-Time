#pragma once

namespace Engine
{
	/* 빛의 정보를 담아놓기 위한 구조체. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_END };

		TYPE			eType; // 빛 타입

		XMFLOAT4		vDirection; // Direct Light

		XMFLOAT4		vDiffuse; // 방향광 색(Direct Light 색)
		XMFLOAT4		vAmbient; // 환경광 색

	}LIGHTDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;






	// D3DDECLUSAGE
	typedef struct tagVertex
	{
		XMFLOAT3		vPosition;
	}VTX;
	typedef struct ENGINE_DLL tagVertex_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTX_DECLARATION;

	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;
	}VTXTEX;
	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;


	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;
	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;




	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;
	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;


	typedef struct tagVertexColor
	{
		XMFLOAT3		vPosition;
		XMFLOAT4		vColor;
	}VTXCOL;
	typedef struct ENGINE_DLL tagVertexCubeColorTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBECOLTEX_DECLARATION;


	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		WINMODE eWinMode;

	}GRAPHICDESC;

}
