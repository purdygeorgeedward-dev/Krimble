
#ifndef KOMERGETEXTPROPERTIESINTOSHAPESCOMMAND_H
#define KOMERGETEXTPROPERTIESINTOSHAPESCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <KoSvgTextProperties.h>

class KoShape;
class KoSvgTextProperties;

class KRITAFLAKE_EXPORT KoShapeMergeTextPropertiesCommand : public KUndo2Command
{
public:
    KoShapeMergeTextPropertiesCommand(const QList<KoShape*> &shapes, const KoSvgTextProperties &props, const QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>(), KUndo2Command *parent = nullptr);
    ~KoShapeMergeTextPropertiesCommand() = default;

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
