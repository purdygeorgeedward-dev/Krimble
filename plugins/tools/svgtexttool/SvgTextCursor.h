/*
 *  SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SVGTEXTCURSOR_H
#define SVGTEXTCURSOR_H

#include <KoSvgTextShape.h>
#include <KoSvgTextProperties.h>
#include <KoSvgTextPropertiesInterface.h>
#include <KoToolSelection.h>
#include <QPainter>
#include <KoShape.h>
#include "kritatoolsvgtext_export.h"
#include <KisHandleStyle.h>

class KoCanvasBase;
class SvgTextInsertCommand;
class SvgTextRemoveCommand;
class KUndo2Command;
class QKeyEvent;
class QInputMethodEvent;
class QAction;

class KRITATOOLSVGTEXT_EXPORT SvgTextCursor : public KoToolSelection, public KoSvgTextShape::TextCursorChangeListener
{
    Q_OBJECT
public:
    explicit SvgTextCursor(KoCanvasBase *canvas);

    enum MoveMode {
        MoveNone,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        MoveNextChar,
        MovePreviousChar,
        MoveNextLine,
        MovePreviousLine,
        MoveWordLeft,
        MoveWordRight,
        MoveWordEnd,
        MoveWordStart,
        MoveLineStart,
        MoveLineEnd,
        ParagraphStart,
        ParagraphEnd,
    };

    enum TypeSettingModeHandle {
        NoHandle,
        StartPos,
        EndPos,

        BaselineShift,
        Ascender,
        Descender,
        LineHeightTop,
        LineHeightBottom,

        BaselineAlphabetic,
        BaselineIdeographic,
        BaselineMiddle,
        BaselineHanging,
        BaselineMathematical,
        BaselineCentral
    };

    ~SvgTextCursor();

    KoSvgTextShape *shape() const;

    void setShape(KoSvgTextShape *textShape);

    void setCaretSetting(int cursorWidth = 1, int cursorFlash = 1000, int cursorFlashLimit = 5000, bool drawCursorInAdditionToSelection = false);

    void setVisualMode(const bool visualMode = true);

    void setPasteRichTextByDefault(const bool pasteRichText = true);

    void setTypeSettingModeActive(bool activate);

    int getPos();

    int getAnchor();

    void setPos(int pos, int anchor);

    void setPosToPoint(QPointF point, bool moveAnchor = true);

    int indexForPoint(QPointF point) const;
    QPointF selectionHandlePos(bool startHandle) const;
    bool hitTestSelectionHandle(QPointF point, qreal sensitivity, bool &isStartHandle) const;
    void setAnchorToPoint(QPointF point);

    TypeSettingModeHandle typeSettingHandleAtPos(const QRectF regionOfInterest);

    void setTypeSettingHandleHovered(TypeSettingModeHandle hovered = TypeSettingModeHandle::NoHandle);

    void setDrawTypeSettingHandle(bool draw);

    void updateTypeSettingDecorFromShape();

    QCursor cursorTypeForTypeSetting() const;

    QString handleName(TypeSettingModeHandle handle) const;

    bool setDominantBaselineFromHandle(const TypeSettingModeHandle handle);

    int posForTypeSettingHandleAndRect(const TypeSettingModeHandle handle, const QRectF regionOfInterest);

    void moveCursor(MoveMode mode, bool moveAnchor = true);

    void insertText(QString text);

    void insertRichText(KoSvgTextShape *insert, bool inheritPropertiesIfPossible = false);

    void removeText(MoveMode first, MoveMode second);

    void removeLastCodePoint();

    QPair<KoSvgTextProperties, KoSvgTextProperties> currentTextProperties() const;

    QList<KoSvgTextProperties> propertiesForRange() const;
    QList<KoSvgTextProperties> propertiesForShape() const;

    void mergePropertiesIntoSelection(const KoSvgTextProperties props, const QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>(), bool paragraphOnly = false, bool selectWord = false);

    void removeSelection();

    void selectAll();

    void copy() const;
    bool paste();

    void deselectText();

    void paintDecorations(QPainter &gc, QColor selectionColor, int decorationThickness = 1, qreal handleRadius = 5.0, KisHandlePalette handlePalette = KisHandlePalette());

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    void inputMethodEvent(QInputMethodEvent *event);

    bool hasSelection() override;

    void notifyShapeChanged(KoShape::ChangeType type, KoShape *shape) override;

    void notifyCursorPosChanged(int pos, int anchor) override;

    void notifyMarkupChanged() override;

    void keyPressEvent(QKeyEvent *event);

    void updateModifiers(const Qt::KeyboardModifiers modifiers);

    bool isAddingCommand() const;

    void focusIn();

    void focusOut();

    bool registerPropertyAction(QAction *action, const QString &name);

    KoSvgTextPropertiesInterface *textPropertyInterface();

Q_SIGNALS:

    void updateCursorDecoration(QRectF updateRect);
    void selectionChanged();

    void sigOpenGlyphPalette();
private Q_SLOTS:
    void blinkCursor();
    void stopBlinkCursor();

    void updateInputMethodItemTransform();
    void canvasResourceChanged(int key, const QVariant &value);
    void propertyAction();
    void clearFormattingAction();

    bool pasteRichText();

    bool pastePlainText();

    void removeTransformsFromRange();

    void updateCanvasResources();

private:

    SvgTextRemoveCommand *removeSelectionImpl(bool allowCleanUp, KUndo2Command *parent = 0);

    void updateCursor(bool firstUpdate = false);
    void updateSelection();
    void updateIMEDecoration();
    void updateTypeSettingDecoration();

    void addCommandToUndoAdapter(KUndo2Command *cmd);

    int moveModeResult(const MoveMode mode, int &pos, bool visual = false) const;

    bool acceptableInput(const QKeyEvent *event) const;

    void commitIMEPreEdit();

    struct Private;
    const QScopedPointer<Private> d;
};

class KRITATOOLSVGTEXT_EXPORT SvgTextCursorPropertyInterface : public KoSvgTextPropertiesInterface
{
public:
    SvgTextCursorPropertyInterface(SvgTextCursor *parent);
    ~SvgTextCursorPropertyInterface();
    virtual QList<KoSvgTextProperties> getSelectedProperties() override;
    virtual QList<KoSvgTextProperties> getCharacterProperties() override;
    virtual KoSvgTextProperties getInheritedProperties() override;
    virtual void setPropertiesOnSelected(KoSvgTextProperties properties, QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>()) override;
    virtual void setCharacterPropertiesOnSelected(KoSvgTextProperties properties, QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>()) override;
    virtual bool spanSelection() override;
    virtual bool characterPropertiesEnabled() override;
    void emitSelectionChange();
    void emitCharacterSelectionChange();
private:
    struct Private;
    const QScopedPointer<Private> d;
};

Q_DECLARE_METATYPE(SvgTextCursor::MoveMode)

#endif
