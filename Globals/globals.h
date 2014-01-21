#ifndef GLOBALS_H
#define GLOBALS_H

#include "qglobal.h"

namespace Globals
{
	enum LANGUAGE
	{
		LANG_EN = 0,
		LANG_HY
	};

	enum AlGO_FLAGS
	{
		ALGO_FLAG_NO_OPTIONS = 0,
		ALGO_FLAG_POINT_PICKER = 1,
		ALGO_FLAG_KNOT_PICKER = 2
	};

	 Q_DECLARE_FLAGS( AlgorithmFlags, AlGO_FLAGS )
	 Q_DECLARE_OPERATORS_FOR_FLAGS( AlgorithmFlags )

	 enum QWT_USER_ITEMS
	 {
		 Rtti_PlotKnot = 1001,
		 Rtti_PlotMarker = 1002
	 };

	 const int SELECTION_PIXEL_TOLERANCE = 10;
}

#endif // GLOBALS_H
