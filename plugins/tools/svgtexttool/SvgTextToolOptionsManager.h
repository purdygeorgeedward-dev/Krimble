/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGTEXTTOOLOPTIONSOBJECT_H
#define SVGTEXTTOOLOPTIONSOBJECT_H

#include <QObject>
#include "SvgTextToolOptionsModel.h"
class SvgTextToolOptionsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SvgTextToolOptionsModel *optionsModel READ optionsModel WRITE setOptionsModel NOTIFY optionsModelChanged)

    Q_PROPERTY(int textType READ textType WRITE convertToTextType NOTIFY convertTextType)
    Q_PROPERTY(bool textPropertiesOpen READ textPropertiesOpen WRITE setTextPropertiesOpen NOTIFY openTextPropertiesDocker)
    Q_PROPERTY(bool showTextPropertyButton READ showTextPropertyButton WRITE setShowTextPropertyButton NOTIFY showTextPropertyButtonChanged)
    Q_PROPERTY(bool typeSettingMode READ typeSettingMode WRITE setTypeSettingMode NOTIFY typeSettingModeChanged)

    Q_PROPERTY(bool showDebug READ showDebug WRITE setShowDebug NOTIFY showDebugChanged)
    Q_PROPERTY(bool showCharacterDebug READ showCharacterDebug WRITE setShowCharacterDebug NOTIFY showDebugCharacterChanged)
    Q_PROPERTY(bool showLineDebug READ showLineDebug WRITE setShowLineDebug NOTIFY showLineDebugChanged)

public:
    explicit SvgTextToolOptionsManager(QObject *parent = nullptr);
    ~SvgTextToolOptionsManager();

    enum TextType {
        PrePositionedText = 0,
        PreformattedText,
        InlineWrap,
        TextInShape
    };
    Q_ENUM(TextType)

    SvgTextToolOptionsModel* optionsModel() const;
    void setOptionsModel(SvgTextToolOptionsModel *model);

    bool showDebug() const;
    void setShowDebug(const bool show);

    bool showCharacterDebug() const;
    void setShowCharacterDebug(const bool show);

    bool showLineDebug() const;
    void setShowLineDebug(const bool show);

    int textType() const;
    void convertToTextType(const int type);

    bool textPropertiesOpen() const;
    void setTextPropertiesOpen(const bool open);

    bool showTextPropertyButton() const;
    void setShowTextPropertyButton(const bool show);

    bool typeSettingMode() const;
    void setTypeSettingMode(const bool activate);

public Q_SLOTS:
    void emitOpenTextEditor();
    void emitGlyphPalette();

Q_SIGNALS:
    void optionsModelChanged();
    void convertTextType(int type);
    void openTextEditor();
    void openGlyphPalette();

    void openTextPropertiesDocker(bool open);
    void showTextPropertyButtonChanged();

    void showDebugChanged();
    void showDebugCharacterChanged();
    void showLineDebugChanged();

    void typeSettingModeChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
