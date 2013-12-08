TEMPLATE = subdirs

#Do not forget!!!
CONFIG += ordered

SUBDIRS += QWT/src/qwt.pro \
    #QWT/examples/event_filter/event_filter/event_filter.pro \
    Globals/Globals.pro \
    Interfaces/Interfaces.pro \
    PolynomialAlgorithm/PolynomialAlgorithm.pro \
    SplineAlgorithm/SplineAlgorithm.pro \
    OptimaPlotter/OptimaPlotter.pro

#QWT/examples/event_filter/event_filter/event_filter.pro.depends =  QWT/src/qwt.pro
Globals/Globals.pro.depends = QWT/src/qwt.pro
Interfaces/Interfaces.pro.depends = Globals/Globals.pro

PolynomialAlgorithm/PolynomialAlgorithm.pro.depends = Interfaces/Interfaces.pro
SplineAlgorithm/SplineAlgorithm.pro.depends = Interfaces/Interfaces.pro
OptimaPlotter/OptimaPlotter.pro.depends = SplineAlgorithm/SplineAlgorithm.pro
OptimaPlotter/OptimaPlotter.pro.depends = PolynomialAlgorithm/PolynomialAlgorithm.pro
