#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CUIManager final : public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	CUIManager();
	virtual ~CUIManager() = default;

public:
	HRESULT Make_InvenUI();
	HRESULT Make_ItemInvenUI();
	HRESULT Make_DiamondUI();

	// For. Inven
public:
	HRESULT Update_HatInvenSlot();
	HRESULT Update_ItemInvenSlot();

	// For. Diamond
public:
	HRESULT Set_Score(_uint iNum);



private:
	class CUI_Inven*		pInven = nullptr;
	class CUI_Item_Inven*	pItem_Inven = nullptr;
	class CUI_DiamondScore* pDiamondScore = nullptr;

public:
	virtual void Free() override;

};

END