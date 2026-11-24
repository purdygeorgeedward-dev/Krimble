/* This file is part of the KDE project
 *
   SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SVG_TEXT_TOOL
#define SVG_TEXT_TOOL

#include <KConfigGroup>
#include <KoToolBase.h>
#include <QPointer>
#include <QElapsedTimer>

#include <KoSvgTextShapeOutlineHelper.h>

#include <kis_signal_auto_connection.h>
#include <KisSignalMapper.h>

#include "SvgTextCursor.h"
#include "SvgTextToolOptionsManager.h"
#include "SvgTextOnPathDecorationHelper.h"
#include "glyphpalette/GlyphPaletteDialog.h"
#include "SvgTextQuickActionBar.h"

#include <memory>

class KoSelection;
class SvgTextEditor;
class KoSvgTextShape;
class KoInteractionStrategy;
class KUndo2Command;
class QActionGroup;

class SvgTextTool : public KoToolBase
{
    Q_OBJECT

    friend class SvgCreateTextStrategy;
    friend class SvgChangeTextPathInfoStrategy;

public:
    explicit SvgTextTool(KoCanvasBase *canvas);
    ~SvgTextTool() override;
    QRectF decorationsRect() const override;
    void paint(QPainter &gc, const KoViewConverter &converter) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void mouseTripleClickEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void activate(const QSet<KoShape *> &shapes) override;
    void deactivate() override;

    KisPopupWidgetInterface* popupWidget() override;

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
    void inputMethodEvent(QInputMethodEvent *event) override;

    void copy() const override;
    void deleteSelection() override;
    bool paste() override;
    bool hasSelection() override;

    bool selectAll() override;

    void deselect() override;
    KoToolSelection * selection() override;

    void requestStrokeEnd() override;
    void requestStrokeCancellation() override;

protected:
    virtual QWidget *createOptionWidget() override;

    KoSelection *koSelection() const;
    KoSvgTextShape *selectedShape() const;

private:
    qreal grabSensitivityInPt() const;

    KoSvgText::WritingMode writingMode() const;

    void addMappedAction(KisSignalMapper* mapper, const QString &actionName, const int value, QActionGroup *group = nullptr);

    bool nodeEditable();

private Q_SLOTS:

    void showEditor();
    void textUpdated(KoSvgTextShape *shape, const QString &svg, const QString &defs);

    void showGlyphPalette();
    void updateGlyphPalette();

    void updateTextPathHelper();

    void updateQuickActionBar();
    void slotQuickActionCut();
    void slotQuickActionCopy();
    void slotQuickActionPaste();
    void slotQuickActionSelectAll();

    void insertRichText(KoSvgTextShape *richText, bool replaceLastGlyph = false);

    QString generateDefs(const KoSvgTextProperties &properties = KoSvgTextProperties());

    KoSvgTextProperties propertiesForNewText() const;

    void slotShapeSelectionChanged();

    void slotUpdateCursorDecoration(QRectF updateRect);

    void slotConvertType(int index);

    void slotUpdateVisualCursor();

    void slotUpdateTextPasteBehaviour();

    void slotTextTypeUpdated();

    void slotMoveTextSelection(int index);

    void slotUpdateTypeSettingMode();

private:
    enum class DragMode {
        None = 0,
        Create,
        Select,
        InlineSizeHandle,
        Move,
        TextPathHandle,
        InShapeOffset,
        TypeSetting,
        SelectionHandle,
    };
    enum class HighlightItem {
        None = 0,
        InlineSizeStartHandle,
        InlineSizeEndHandle,
        MoveBorder,
        TypeSettingHandle,
    };

    QScopedPointer<SvgTextToolOptionsManager>m_optionManager;
    QPointer<SvgTextEditor> m_editor;
    KoSvgTextShape *m_krimbleEditorShape {nullptr};
    QPointer<GlyphPaletteDialog> m_glyphPalette;
    QPointer<SvgTextQuickActionBar> m_quickActionBar;
    QPointF m_lastMousePos;
    DragMode m_dragging {DragMode::None};
    std::unique_ptr<KoInteractionStrategy> m_interactionStrategy;
    HighlightItem m_highlightItem {HighlightItem::None};
    bool m_strategyAddingCommand {false};

    QScopedPointer<KisSignalMapper> m_textTypeSignalsMapper;
    QScopedPointer<KisSignalMapper> m_typeSettingMovementMapper;

    QPointF m_lastDoubleClickPos;
    QElapsedTimer m_lastDoubleClickElapsed;

    SvgTextCursor m_textCursor;
    SvgTextOnPathDecorationHelper m_textOnPathHelper;
    QScopedPointer<KoSvgTextShapeOutlineHelper> m_textOutlineHelper;
    KisSignalAutoConnectionsStore m_canvasConnections;

    QPainterPath m_hoveredShapeHighlightRect;

    QCursor m_base_cursor;
    QCursor m_text_inline_horizontal;
    QCursor m_text_inline_vertical;
    QCursor m_text_on_path;
    QCursor m_text_in_shape;
    QCursor m_ibeam_vertical;
    QCursor m_ibeam_horizontal;
    QCursor m_ibeam_horizontal_done;

};

#endif
