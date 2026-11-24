#ifndef KIS_ABSTRACT_RESOURCES_H
#define KIS_ABSTRACT_RESOURCES_H

#endif

#include "KoAbstractCanvasResourceInterface.h"

class ToolOpacityAbstractResource : public KoAbstractCanvasResourceInterface
{
public:
    ToolOpacityAbstractResource(int key, qreal value);

    QVariant value() const override;
    void setValue(const QVariant value) override;

private:
    qreal m_value;
};
