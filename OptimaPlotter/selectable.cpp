#include "stdafx.h"
#include "selectable.h"

Selectable::Selectable() : m_stateOfSelection( false )
{

}

Selectable::~Selectable()
{

}

void Selectable::setSelectionState( bool stateOfSelection )
{
	stateOfSelection ? select() : deselect();
}

void Selectable::select()
{
	m_stateOfSelection = true;
}

void Selectable::deselect()
{
	m_stateOfSelection = false;
}

bool Selectable::isSelected() const
{
	return m_stateOfSelection;
}