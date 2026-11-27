/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_KRA_LOADER_H
#define KIS_KRA_LOADER_H

#include <QStringList>
#include <QString>
#include "QDomDocument"

class KoStore;

class KisDocument;
class KoColorSpace;
class KisPaintingAssistant;
class StoryboardComment;
class QVersionNumber;

#include <kis_types.h>
#include "kritalibkra_export.h"
#include "KoColor.h"

class KRITALIBKRA_EXPORT KisKraLoader
{

public:

    KisKraLoader(KisDocument * document, int syntaxVersion, const QVersionNumber &kritaVersion);

    ~KisKraLoader();

    KisImageSP loadXML(const QDomElement& imageElement);

    void loadBinaryData(KoStore* store, KisImageSP image, const QString & uri, bool external);

    void loadResources(KoStore *store, KisDocument *doc);
    void loadStoryboards(KoStore *store, KisDocument *doc);
    void loadAnimationMetadata(KoStore *store, KisImageSP image);
    void loadAudio(KoStore *store, KisDocument *kisDoc);
    Q_DECL_DEPRECATED void backCompat_loadAudio(const QDomElement &elem, KisDocument *document);

    vKisNodeSP selectedNodes() const;

    QList<KisPaintingAssistantSP> assistants() const;

    StoryboardItemList storyboardItemList() const;

    StoryboardCommentList storyboardCommentList() const;

    QStringList errorMessages() const;

    QStringList warningMessages() const;

    QString imageName() const;

private:

    void loadAssistants(KoStore* store, const QString & uri, bool external);

    void loadAnimationMetadataFromXML(const QDomElement& element, KisImageSP image);

    KisNodeSP loadNodes(const QDomElement& element, KisImageSP image, KisNodeSP parent);

    KisNodeSP loadNode(const QDomElement& elem, KisImageSP image);

    KisNodeSP loadPaintLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadGroupLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadAdjustmentLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadShapeLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadGeneratorLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadCloneLayer(const QDomElement& elem, KisImageSP image, const QString& name, const KoColorSpace* cs, quint32 opacity);

    KisNodeSP loadFilterMask(KisImageSP image, const QDomElement& elem);

    KisNodeSP loadTransformMask(KisImageSP image, const QDomElement& elem);

    KisNodeSP loadTransparencyMask(KisImageSP image, const QDomElement& elem);

    KisNodeSP loadSelectionMask(KisImageSP image, const QDomElement& elem);

    KisNodeSP loadColorizeMask(KisImageSP image, const QDomElement& elem, const KoColorSpace *colorSpace);

    KisNodeSP loadFileLayer(const QDomElement& elem, KisImageSP image, const QString& name, quint32 opacity, const KoColorSpace *fallbackColorSpace);

    KisNodeSP loadReferenceImagesLayer(const QDomElement& elem, KisImageSP image);

    void loadNodeKeyframes(KoStore *store, const QString &location, KisNodeSP node);

    void loadCompositions(const QDomElement& elem, KisImageSP image);

    void loadAssistantsList(const QDomElement& elem);
    void loadGrid(const QDomElement& elem);
    void loadGuides(const QDomElement& elem);
    void loadMirrorAxis(const QDomElement& elem);
    void loadStoryboardItemList(const QDomElement& elem);
    void loadStoryboardCommentList(const QDomElement& elem);
    void loadAudioXML(QDomDocument& xmlDoc, QDomElement &xmlElement, KisDocument* kisDoc);
    QList<KoColor> loadKoColors(const QDomElement& elem) const;
private:

    struct Private;
    Private * const m_d;

};

#endif
