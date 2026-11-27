/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSHAPEQTQUICKLABEL_H
#define KOSHAPEQTQUICKLABEL_H

#include <QQuickPaintedItem>

class KoShapeQtQuickLabel: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString svgData READ svgData WRITE setSvgData NOTIFY svgDataChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(int padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(bool fullColor READ fullColor WRITE setFullColor NOTIFY fullColorChanged)
    Q_PROPERTY(ScalingType scalingType READ scalingType WRITE setScalingType NOTIFY scalingTypeChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(QRectF documentRect READ documentRect WRITE setDocumentRect NOTIFY documentRectChanged)
    Q_PROPERTY(QRectF minimumRect READ minimumRect NOTIFY minimumRectChanged)

    QML_NAMED_ELEMENT(KoShapeQtQuickLabel)
public:
    KoShapeQtQuickLabel(QQuickItem *parent = nullptr);
    ~KoShapeQtQuickLabel();

    enum ScalingType {
        Fit,
        FitWidth,
        FitHeight
    };
    Q_ENUM(ScalingType)

public:
    void paint(QPainter *painter) override;
    QString svgData() const;
    void setSvgData(const QString &newSvgData);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor &newForegroundColor);

    int padding() const;
    void setPadding(int newPadding);

    bool fullColor() const;
    void setFullColor(bool newFullColor);

    ScalingType scalingType() const;
    void setScalingType(const ScalingType type);

    Qt::Alignment alignment() const;
    void setAlignment(const Qt::Alignment align);

    QRectF documentRect() const;
    void setDocumentRect(const QRectF &rect);

    QRectF minimumRect() const;

    void componentComplete() override;

Q_SIGNALS:
    void svgDataChanged();

    void fillColorChanged();

    void foregroundColorChanged();

    void paddingChanged();

    void fullColorChanged();

    void scalingTypeChanged();

    void alignmentChanged();

    void documentRectChanged();

    void minimumRectChanged();

private Q_SLOTS:
    void updateShapes();

    void callUpdateIfComplete();

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
