#ifndef KISROLLINGSUMACCUMULATORWRAPPER_H
#define KISROLLINGSUMACCUMULATORWRAPPER_H

#include <QtGlobal>
#include <QScopedPointer>
#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KisRollingSumAccumulatorWrapper
{
public:
    KisRollingSumAccumulatorWrapper(int windowSize);
    ~KisRollingSumAccumulatorWrapper();

    void operator()(qreal value);

    qreal rollingSum() const;

    int rollingCount() const;

    void reset(int windowSize);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
