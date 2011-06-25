#ifndef _UICREATOR_
#define _UICREATOR_
#pragma once

// todo: look here, this is how it should work

#include "Interfaces.h"

struct UIData;
struct UICreator : public DistributedCreator<boost::tuple<UIData*,UI*>>
{
	typedef boost::tuple<UIData*,UI*> TupleType;
};

class CheckBoxCreator : public UICreator
{
public:

	virtual IGameEntity* Create(const TupleType& t) const;

};

class TextBoxCreator : public UICreator
{
public:

	virtual IGameEntity* Create(const TupleType& t) const;

};

#endif // _UICREATOR_