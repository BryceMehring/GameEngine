#include "stdafx.h"
#include "UICreator.h"
#include "UI.h"


IObject* CheckBoxCreator::Create(const TupleType& t) const
{
	CheckBoxData* pData = static_cast<CheckBoxData*>(t.get<0>());
	UI* pData2 = t.get<1>();

	return new CheckBox(pData,pData2);
	//return new CheckBox(pData,pUI);
}

IObject* TextBoxCreator::Create(const TupleType& t) const
{
	UIData* pData = t.get<0>();
	UI* pData2 = t.get<1>();

	return new TextBox(pData,pData2);
}