OptimaPlotter
=============

Overview
=============

Plots the least squares solution in polynomial and spline spaces using as input user-marked markers ( and knots in spline mode ). Allowed degrees for polynomial and splines are 1 ... 4.

Features
=============

  - Interactive plot grid with panner and zoomer;
  - Marker selector;
  - Knot picker ( only in spline mode );
  - Expandable via plug-ins;
  - 2 languages support ( English/ Հայերեն ).
  
Building
=============

The solution is bundled with QWT and Eigen libraries.
To build the solution you'll need Qt 4.8 ( other versions are not tested ).

Windows users can build the solution using both Qt Creator and VS 2010 ( *.pro and *.sln files are provided simultaneously ).
Linux/ Mac users may need slight changes in *.pro file concerning the library linkage.
