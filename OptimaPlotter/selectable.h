#ifndef SELECTABLE_H
#define SELECTABLE_H

class Selectable
{
public:
	Selectable();
	virtual ~Selectable();

	virtual void select();
	virtual void deselect();
	void setSelectionState( bool stateOfSelection );
	
	bool isSelected() const;

private:
	bool m_stateOfSelection;
};

#endif