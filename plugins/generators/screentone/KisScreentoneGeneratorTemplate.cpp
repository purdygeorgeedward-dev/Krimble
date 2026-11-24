/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <algorithm>
#include <limits>
#include <cmath>

#include "KisScreentoneScreentoneFunctions.h"
#include "KisScreentoneGeneratorTemplate.h"

KisScreentoneGeneratorTemplate::KisScreentoneGeneratorTemplate(const KisScreentoneGeneratorConfigurationSP config)
{
    const int pattern = config->pattern();
    const int shape = config->shape();
    const int interpolation = config->interpolation();

    if (pattern == KisScreentonePatternType_Dots) {
        if (shape == KisScreentoneShapeType_RoundDots) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::DotsRoundLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::DotsRoundSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_EllipseDotsLegacy) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::DotsEllipseLinear_Legacy screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::DotsEllipseSinusoidal_Legacy screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_EllipseDots) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::DotsEllipseLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::DotsEllipseSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_DiamondDots) {
            KisScreentoneScreentoneFunctions::DotsDiamond screentoneFunction;
            makeTemplate(config, screentoneFunction);
        } else if (shape == KisScreentoneShapeType_SquareDots) {
            KisScreentoneScreentoneFunctions::DotsSquare screentoneFunction;
            makeTemplate(config, screentoneFunction);
        }
    } else if (pattern == KisScreentonePatternType_Lines) {
        if (shape == KisScreentoneShapeType_StraightLines) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::LinesStraightLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::LinesStraightSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_SineWaveLines) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::LinesSineWaveLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::LinesSineWaveSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_TriangularWaveLines) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::LinesTriangularWaveLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::LinesTriangularWaveSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_SawtoothWaveLines) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::LinesSawToothWaveLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::LinesSawToothWaveSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        } else if (shape == KisScreentoneShapeType_CurtainsLines) {
            if (interpolation == KisScreentoneInterpolationType_Linear) {
                KisScreentoneScreentoneFunctions::LinesCurtainsLinear screentoneFunction;
                makeTemplate(config, screentoneFunction);
            } else if (interpolation == KisScreentoneInterpolationType_Sinusoidal) {
                KisScreentoneScreentoneFunctions::LinesCurtainsSinusoidal screentoneFunction;
                makeTemplate(config, screentoneFunction);
            }
        }
    }
}

template <typename ScreentoneFunction>
struct LightUpOrderingFunction : public ScreentoneFunction
{};
template <>
struct LightUpOrderingFunction<KisScreentoneScreentoneFunctions::DotsRoundSinusoidal>
    : public KisScreentoneScreentoneFunctions::DotsRoundLinear
{};
template <>
struct LightUpOrderingFunction<KisScreentoneScreentoneFunctions::DotsEllipseSinusoidal>
    : public KisScreentoneScreentoneFunctions::DotsEllipseLinear
{};

template <typename ScreentoneFunction>
void KisScreentoneGeneratorTemplate::makeTemplate(const KisScreentoneGeneratorConfigurationSP config,
                                                  ScreentoneFunction screentoneFunction)
{

    qreal sizeX, sizeY;
    const int alignX = config->alignToPixelGrid() ? config->alignToPixelGridX() : 1;
    const int alignY = config->alignToPixelGrid() ? config->alignToPixelGridY() : 1;
    if (config->sizeMode() == KisScreentoneSizeMode_PixelBased) {
        const bool constrainSize = config->constrainSize();
        sizeX = config->sizeX();
        sizeY = constrainSize ? sizeX : config->sizeY();
    } else {
        const qreal resolution = config->resolution();
        const bool constrainFrequency = config->constrainFrequency();
        const qreal frequencyX = config->frequencyX();
        const qreal frequencyY = constrainFrequency ? frequencyX : config->frequencyY();
        sizeX = qMax(1.0, resolution / frequencyX);
        sizeY = qMax(1.0, resolution / frequencyY);
    }
    const qreal positionX = config->alignToPixelGrid() ? qRound(config->positionX()) : config->positionX();
    const qreal positionY = config->alignToPixelGrid() ? qRound(config->positionY()) : config->positionY();
    m_screenPosition = QPointF(positionX, positionY);
    const qreal shearX = config->shearX();
    const qreal shearY = config->shearY();
    const qreal rotation = config->rotation();
    m_imageToScreenTransform.shear(shearX, shearY);
    m_imageToScreenTransform.scale(qFuzzyIsNull(sizeX) ? 0.0 : 1.0 / sizeX, qFuzzyIsNull(sizeY) ? 0.0 : 1.0 / sizeY);
    m_imageToScreenTransform.rotate(rotation);
    m_imageToScreenTransform.translate(positionX, positionY);
    QTransform screenToImage;
    screenToImage.rotate(-rotation);
    screenToImage.scale(sizeX, sizeY);
    screenToImage.shear(-shearX, -shearY);
    const QPointF u1 = screenToImage.map(QPointF(static_cast<qreal>(alignX), 0.0));
    const QPointF u2 = screenToImage.map(QPointF(0.0, static_cast<qreal>(alignY)));
    QPointF v1(qRound(u1.x()), qRound(u1.y()));
    QPointF v2(qRound(u2.x()), qRound(u2.y()));
    if (qFuzzyCompare(v1.y() * v2.x(), v2.y() * v1.x()) &&
        !qFuzzyIsNull(v1.x() * v2.x() + v1.y() * v2.y())) {
        const qreal dist1 = kisSquareDistance(u1, v1);
        const qreal dist2 = kisSquareDistance(u2, v2);
        const QPointF *p_u = dist1 > dist2 ? &u1 : &u2;
        QPointF *p_v = dist1 > dist2 ? &v1 : &v2;
        QPair<int, qreal> dists[4]{
            {1, kisSquareDistance(*p_u, *p_v + QPointF(0.0, -1.0))},
            {2, kisSquareDistance(*p_u, *p_v + QPointF(1.0, 0.0))},
            {3, kisSquareDistance(*p_u, *p_v + QPointF(0.0, 1.0))},
            {4, kisSquareDistance(*p_u, *p_v + QPointF(-1.0, 0.0))}
        };
        std::sort(
            std::begin(dists), std::end(dists),
            [](const QPair<int, qreal> &a, const QPair<int, qreal> &b)
            {
                return a.second < b.second;
            }
        );
        if (dists[0].first == 1) {
            p_v->setY(p_v->y() - 1.0);
        } else if (dists[0].first == 2) {
            p_v->setX(p_v->x() + 1.0);
        } else if (dists[0].first == 3) {
            p_v->setY(p_v->y() + 1.0);
        } else {
            p_v->setX(p_v->x() - 1.0);
        }
    }
    qreal v1Length = std::sqrt(v1.x() * v1.x() + v1.y() * v1.y());
    qreal v2Length = std::sqrt(v2.x() * v2.x() + v2.y() * v2.y());
    QSize macrocellTiles(1, 1);
    while (macrocellTiles.width() * v1Length * macrocellTiles.height() * v2Length < 256) {
        if (macrocellTiles.width() * v1Length > macrocellTiles.height() * v2Length) {
            macrocellTiles.setHeight(macrocellTiles.height() + 1);
        } else {
            macrocellTiles.setWidth(macrocellTiles.width() + 1);
        }
    }
    m_macrocellSize = QSize(alignX * macrocellTiles.width(), alignY * macrocellTiles.height());
    const QSizeF macrocellSizeF = m_macrocellSize;
    v1 *= macrocellTiles.width();
    v2 *= macrocellTiles.height();
    v1Length *= macrocellTiles.width();
    v2Length *= macrocellTiles.height();
    const QPointF v3 = v1 + v2;
    const QPointF l1 = v1;
    const QPointF l2 = v2;
    const QPointF l3 = -v1;
    const QPointF l4 = -v2;
    const qreal v1MicrocellLength = v1Length / macrocellSizeF.width();
    const qreal v2MicrocellLength = v2Length / macrocellSizeF.height();
    m_v1 = v1;
    m_v2 = v2;
    const QPolygonF quad(
        {
            QPointF(0.0, 0.0),
            v1 / macrocellSizeF.width(),
            v1 / macrocellSizeF.width() + v2 / macrocellSizeF.height(),
            v2 / macrocellSizeF.height()
        }
    );
    QTransform::quadToSquare(quad, m_templateToScreenTransform);
    m_screenToTemplateTransform = m_templateToScreenTransform.inverted();

    const QPoint topLeft(
        static_cast<int>(qMin(0.0, qMin(v1.x(), qMin(v2.x(), v1.x() + v2.x())))),
        static_cast<int>(qMin(0.0, qMin(v1.y(), qMin(v2.y(), v1.y() + v2.y()))))
    );
    const QPoint bottomRight(
        static_cast<int>(qMax(0.0, qMax(v1.x(), qMax(v2.x(), v1.x() + v2.x())))),
        static_cast<int>(qMax(0.0, qMax(v1.y(), qMax(v2.y(), v1.y() + v2.y()))))
    );
    m_templateSize = QSize(bottomRight.x() - topLeft.x() + 2, bottomRight.y() - topLeft.y() + 2);
    m_originOffset = -topLeft + QPoint(1, 1);

    m_templateData = QVector<qreal>(m_templateSize.width() * m_templateSize.height(), -1.0);

    struct AuxiliaryPoint
    {
        int templatePixelIndex;
        qreal microcellPixelIndex;
        qreal valueAtCorner;
        qreal valueAtCenter;
        qreal distanceToCorner;
        qreal distanceToCenter;
    };
    struct AuxiliaryMicrocell
    {
        int index;
        QVector<AuxiliaryPoint> auxiliaryPoints;
    };

    QVector<AuxiliaryMicrocell> auxiliaryMicrocells(m_macrocellSize.width() * m_macrocellSize.height());

    QVector<int> microcellIndices = makeCellOrderList(m_macrocellSize.width(), m_macrocellSize.height());
    for (int i = 0; i < auxiliaryMicrocells.size(); ++i) {
        auxiliaryMicrocells[i].index = microcellIndices[i];
    }

    QVector<QPointF> horizontalCellPositions;
    for (int i = -1; i <= m_macrocellSize.width() + 1; ++i) {
        horizontalCellPositions.append(v1 / static_cast<qreal>(m_macrocellSize.width()) * static_cast<qreal>(i));
    }
    QVector<QPointF> verticalCellPositions;
    for (int i = -1; i <= m_macrocellSize.height() + 1; ++i) {
        verticalCellPositions.append(v2 / static_cast<qreal>(m_macrocellSize.height()) * static_cast<qreal>(i));
    }

    int totalNumberOfAuxiliaryPoints = 0;

    auto pointBelongsToLeftSideOfEdge = [](const QPointF &point, const QPointF &edgeStart, const QPointF &edgeDirection) -> bool
    {
        const qreal r = (point.x() - edgeStart.x()) * edgeDirection.y() - (point.y() - edgeStart.y()) * edgeDirection.x();
        return (r == 0.0) && ((edgeDirection.y() == 0.0 && edgeDirection.x() > 0.0) || (edgeDirection.y() > 0.0));
    };

    LightUpOrderingFunction<ScreentoneFunction> lightUpOrderingFunction;

    for (int i = 0; i < m_templateData.size(); ++i) {
        const int templateY = i / m_templateSize.width();
        const int templateX = i - m_templateSize.width() * templateY;
        const QPointF pixelCornerPoint(
            static_cast<qreal>(templateX - m_originOffset.x()),
            static_cast<qreal>(templateY - m_originOffset.y())
        );
        const QPointF pixelCenterPoint(pixelCornerPoint + QPointF(0.5, 0.5));
        const QPointF pixelCenterScreenPoint = m_templateToScreenTransform.map(pixelCenterPoint);
        const QPointF macrocellPos(pixelCenterScreenPoint.x() * v1MicrocellLength, pixelCenterScreenPoint.y() * v2MicrocellLength);
        int microcellX = static_cast<int>(std::floor(macrocellPos.x() * static_cast<qreal>(m_macrocellSize.width()) / v1Length));
        int microcellY = static_cast<int>(std::floor(macrocellPos.y() * static_cast<qreal>(m_macrocellSize.height()) / v2Length));
        if (microcellX < -1 || microcellX > m_macrocellSize.width() ||
            microcellY < -1 || microcellY > m_macrocellSize.height()) {
            continue;
        }
        const QPointF topLeftCorner(horizontalCellPositions[microcellX + 1] + verticalCellPositions[microcellY + 1]);
        const QPointF topRightCorner(horizontalCellPositions[microcellX + 1 + 1] + verticalCellPositions[microcellY + 1]);
        const QPointF bottomRightCorner(horizontalCellPositions[microcellX + 1 + 1] + verticalCellPositions[microcellY + 1 + 1]);
        const QPointF bottomLeftCorner(horizontalCellPositions[microcellX + 1] + verticalCellPositions[microcellY + 1 + 1]);
        if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, topLeftCorner, l1)) {
            --microcellY;
        }
        if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, topRightCorner, l2)) {
            ++microcellX;
        }
        if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, bottomRightCorner, l3)) {
            ++microcellY;
        }
        if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, bottomLeftCorner, l4)) {
            --microcellX;
        }
        if (microcellX < 0 || microcellX >= m_macrocellSize.width() ||
            microcellY < 0 || microcellY >= m_macrocellSize.height()) {
            continue;
        }
        const int microcellIndex = microcellY * m_macrocellSize.width() + microcellX;
        const qreal microcellPixelIndexX = macrocellPos.x() - static_cast<qreal>(microcellX) * v1MicrocellLength;
        const qreal microcellPixelIndexY = macrocellPos.y() - static_cast<qreal>(microcellY) * v2MicrocellLength;
        const qreal microcellPixelIndex = microcellPixelIndexY * v1MicrocellLength + microcellPixelIndexX;
        const QPointF pixelCornerScreenPoint = m_templateToScreenTransform.map(pixelCornerPoint);
        auxiliaryMicrocells[microcellIndex].auxiliaryPoints.push_back(
            {
                i,
                microcellPixelIndex,
                screentoneFunction(pixelCornerScreenPoint.x(), pixelCornerScreenPoint.y()),
                screentoneFunction(pixelCenterScreenPoint.x(), pixelCenterScreenPoint.y()),
                lightUpOrderingFunction(pixelCornerScreenPoint.x(), pixelCornerScreenPoint.y()),
                lightUpOrderingFunction(pixelCenterScreenPoint.x(), pixelCenterScreenPoint.y()),
            }
        );
        ++totalNumberOfAuxiliaryPoints;
    }

    for (int i = 0; i < auxiliaryMicrocells.size(); ++i) {
        std::sort(auxiliaryMicrocells[i].auxiliaryPoints.begin(), auxiliaryMicrocells[i].auxiliaryPoints.end(),
            [](const AuxiliaryPoint &a, const AuxiliaryPoint &b) {
                if (qFuzzyCompare(a.valueAtCorner, b.valueAtCorner)) {
                    if (qFuzzyCompare(a.valueAtCenter, b.valueAtCenter)) {
                        if (qFuzzyCompare(a.distanceToCenter, b.distanceToCenter)) {
                            if (qFuzzyCompare(a.distanceToCorner, b.distanceToCorner)) {
                                return a.microcellPixelIndex < b.microcellPixelIndex;
                            }
                            return a.distanceToCorner < b.distanceToCorner;
                        }
                        return a.distanceToCenter < b.distanceToCenter;
                    }
                    return a.valueAtCenter < b.valueAtCenter;
                }
                return a.valueAtCorner < b.valueAtCorner;
            }
        );
    }

    std::sort(auxiliaryMicrocells.begin(), auxiliaryMicrocells.end(),
        [](const AuxiliaryMicrocell &a, const AuxiliaryMicrocell &b) {
            return a.index < b.index;
        }
    );

    {
        int macrocellPixelIndex = 0;
        QVector<int> microcellPixelIndices(auxiliaryMicrocells.size());
        while (macrocellPixelIndex < totalNumberOfAuxiliaryPoints) {
            for (int i = 0; i < auxiliaryMicrocells.size(); ++i) {
                if (microcellPixelIndices[i] == auxiliaryMicrocells[i].auxiliaryPoints.size()) {
                    continue;
                }
                m_templateData[auxiliaryMicrocells[i].auxiliaryPoints[microcellPixelIndices[i]].templatePixelIndex] =
                    totalNumberOfAuxiliaryPoints - 1 == 0 ? 0
                                                          : static_cast<qreal>(macrocellPixelIndex) / static_cast<qreal>(totalNumberOfAuxiliaryPoints - 1);
                ++microcellPixelIndices[i];
                ++macrocellPixelIndex;
            }
        }
    }

    for (int i = 0; i < m_templateData.size(); ++i) {
        if (m_templateData[i] < 0.0) {
            int templateY = i / m_templateSize.width();
            int templateX = i - m_templateSize.width() * templateY;
            QPointF pixelCenterPoint(
                static_cast<qreal>(templateX - m_originOffset.x()) + 0.5,
                static_cast<qreal>(templateY - m_originOffset.y()) + 0.5
            );
            const QPointF pixelCenterScreenPoint = m_templateToScreenTransform.map(pixelCenterPoint);
            const qreal a = -std::floor(pixelCenterScreenPoint.x() / macrocellSizeF.width());
            const qreal b = -std::floor(pixelCenterScreenPoint.y() / macrocellSizeF.height());
            pixelCenterPoint += QPointF(a * v1.x() + b * v2.x(), a * v1.y() + b * v2.y());

            int x = static_cast<int>(std::floor(pixelCenterPoint.x())) + m_originOffset.x();
            int y = static_cast<int>(std::floor(pixelCenterPoint.y())) + m_originOffset.y();
            int macrocellPointIndex = y * m_templateSize.width() + x;

            if (m_templateData[macrocellPointIndex] < 0.0) {
                if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, QPointF(0.0, 0.0), l1)) {
                    pixelCenterPoint += v2;
                }
                if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, v1, l2)) {
                    pixelCenterPoint -= v1;
                }
                if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, v3, l3)) {
                    pixelCenterPoint -= v2;
                }
                if (pointBelongsToLeftSideOfEdge(pixelCenterPoint, v2, l4)) {
                    pixelCenterPoint += v1;
                }
                templateX = static_cast<int>(std::floor(pixelCenterPoint.x())) + m_originOffset.x();
                templateY = static_cast<int>(std::floor(pixelCenterPoint.y())) + m_originOffset.y();
                macrocellPointIndex = templateY * m_templateSize.width() + templateX;
            }

            m_templateData[i] = m_templateData[macrocellPointIndex];
        }
    }
}

QVector<int> KisScreentoneGeneratorTemplate::makeCellOrderList(int macrocellColumns, int macrocellRows) const
{
    if (macrocellColumns == 1 && macrocellRows == 1) {
        return {0};
    }

    struct Candidate
    {
        int availableCellIndex;
        int index;
        int distanceSquared;
        int averageDistance;
    };

    const int numberOfCells = macrocellColumns * macrocellRows;
    QVector<int> processedCells;
    QVector<int> availableCells;

    processedCells.push_back(0);
    for (int i = 1; i < numberOfCells; ++i) {
        availableCells.push_back(i);
    }

    while (availableCells.size() > 0) {
        QVector<Candidate> candidates;
        for (int i = 0; i < availableCells.size(); ++i) {
            const int availableCellY = availableCells[i] / macrocellColumns;
            const int availableCellX = availableCells[i] - availableCellY * macrocellColumns;
            int averageDistanceToProcessedCells = 0;
            int minimumDistanceToProcessedCells = std::numeric_limits<int>::max();
            for (int j = 0; j < processedCells.size(); ++j) {
                const int processedCellY = processedCells[j] / macrocellColumns;
                const int processedCellX = processedCells[j] - processedCellY * macrocellColumns;

                int distanceToCurrentProcessedCell = std::numeric_limits<int>::max();
                for (int y = -1; y < 2; ++y) {
                    for (int x = -1; x < 2; ++x) {
                        const int xx = processedCellX + macrocellColumns * x;
                        const int yy = processedCellY + macrocellRows * y;
                        const int deltaX = availableCellX - xx;
                        const int deltaY = availableCellY - yy;
                        const int distanceSquared = deltaX * deltaX + deltaY * deltaY;
                        if (distanceSquared < distanceToCurrentProcessedCell) {
                            distanceToCurrentProcessedCell = distanceSquared;
                        }
                    }
                }
                if (distanceToCurrentProcessedCell < minimumDistanceToProcessedCells) {
                    minimumDistanceToProcessedCells = distanceToCurrentProcessedCell;
                }
                averageDistanceToProcessedCells += distanceToCurrentProcessedCell;
            }
            candidates.push_back({i, availableCells[i], minimumDistanceToProcessedCells, averageDistanceToProcessedCells});
        }

        int bestCandidateIndex = 0;
        for (int i = 0; i < candidates.size(); ++i) {
            if (candidates[i].distanceSquared > candidates[bestCandidateIndex].distanceSquared ||
                (candidates[i].distanceSquared == candidates[bestCandidateIndex].distanceSquared &&
                candidates[i].averageDistance > candidates[bestCandidateIndex].averageDistance)) {
                bestCandidateIndex = i;
            }
        }

        processedCells.push_back(candidates[bestCandidateIndex].index);
        availableCells.remove(candidates[bestCandidateIndex].availableCellIndex);
    }

    QVector<int> cellOrderList(numberOfCells);
    for (int i = 1; i < numberOfCells; ++i) {
        cellOrderList[processedCells[i]] = i;
    }

    return cellOrderList;
}
