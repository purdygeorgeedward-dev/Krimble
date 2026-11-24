/*
 *  SPDX-FileCopyrightText: 2007-2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_CONVERSION_SYSTEM_H_
#define _KO_COLOR_CONVERSION_SYSTEM_H_

class KoColorProfile;
struct KoColorProfileQuery;
class KoColorSpace;
class KoColorSpaceFactory;
class KoColorSpaceEngine;
class KoID;

#include "KoColorConversionTransformation.h"

#include <QList>
#include <QPair>

#include "kritapigment_export.h"

class KRITAPIGMENT_EXPORT KoColorConversionSystem
{
public:
    struct RegistryInterface {
        virtual ~RegistryInterface() {}

        virtual const KoColorSpace * colorSpace(const QString & colorModelId, const QString & colorDepthId, const QString &profileName) = 0;
        virtual const KoColorSpaceFactory* colorSpaceFactory(const QString &colorModelId, const QString &colorDepthId) const = 0;
        virtual QList<const KoColorProfile *>  profilesFor(const KoColorSpaceFactory * csf) const = 0;
        virtual QList<const KoColorSpaceFactory*> colorSpacesFor(const KoColorProfile* profile) const = 0;
    };

public:
    struct Node;
    struct Vertex;
    struct NodeKey;
    friend uint qHash(const KoColorConversionSystem::NodeKey &key);
    struct Path;
    KoColorConversionSystem(RegistryInterface *registryInterface);
    ~KoColorConversionSystem();

    void insertColorSpace(const KoColorSpaceFactory *csf);

    void insertColorProfile(const KoColorProfile *profile);

    QList<KoColorProfileQuery> requiredConnectionProfilesFor(const KoColorSpaceFactory* csf);

    QList<KoColorProfileQuery> requiredConnectionProfilesFor(const KoColorProfile* profile);

    KoColorConversionTransformation* createColorConverter(const KoColorSpace * srcColorSpace, const KoColorSpace * dstColorSpace, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const;

    void createColorConverters(const KoColorSpace* colorSpace, const QList< QPair<KoID, KoID> >& possibilities, KoColorConversionTransformation*& fromCS, KoColorConversionTransformation*& toCS) const;
public:
    QString toDot() const;
    QString bestPathToDot(const QString& srcKey, const QString& dstKey) const;
public:
    bool existsPath(const QString& srcModelId, const QString& srcDepthId, const QString& srcProfileName, const QString& dstModelId, const QString& dstDepthId, const QString& dstProfileName) const;
    bool existsGoodPath(const QString& srcModelId, const QString& srcDepthId, const QString& srcProfileName, const QString& dstModelId, const QString& dstDepthId, const QString& dstProfileName) const;

    Path findBestPath(const QString& srcModelId, const QString& srcDepthId, const QString& srcProfileName, const QString& dstModelId, const QString& dstDepthId, const QString& dstProfileName) const;

    Path findBestPath(const NodeKey &src, const NodeKey &dst) const;
private:
    QString vertexToDot(Vertex* v, const QString &options) const;
private:
    Node* insertEngine(const KoColorSpaceEngine* engine);
    KoColorConversionTransformation* createTransformationFromPath(const KoColorConversionSystem::Path& path, const KoColorSpace* srcColorSpace, const KoColorSpace* dstColorSpace, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) const;
    const KoColorSpace* defaultColorSpaceForNode(const Node* node) const;
    Node* createNode(const QString& _modelId, const QString& _depthId, const QString& _profileName);
    void connectToEngine(Node* _node, Node* _engine);
    const Node* nodeFor(const KoColorSpace*) const;
    Node* nodeFor(const NodeKey& key);
    const Node* nodeFor(const NodeKey& key) const;
    QList<Node*> nodesFor(const QString& _modelId, const QString& _depthId);
    Node* nodeFor(const QString& colorModelId, const QString& colorDepthId, const QString& _profileName);
    const Node* nodeFor(const QString& colorModelId, const QString& colorDepthId, const QString& _profileName) const;
    Vertex* vertexBetween(Node* srcNode, Node* dstNode);
    Vertex* createVertex(Node* srcNode, Node* dstNode);
    Path findBestPath(const Node* srcNode, const Node* dstNode) const;
    void deletePaths(QList<KoColorConversionSystem::Path*> paths) const;

private:
    struct Private;
    Private* const d;
};

#endif
