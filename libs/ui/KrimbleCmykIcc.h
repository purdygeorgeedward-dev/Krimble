#ifndef KRIMBLECMYKICC_H
#define KRIMBLECMYKICC_H

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QEventLoop>
#include <QProgressDialog>
#include <QByteArray>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QWidget>
#include <cmath>
#include <functional>
#include <vector>

#include <KoColorSpaceEngine.h>
#include <KoResourcePaths.h>
#include "KrimbleCmykEngine.h"

namespace KrimbleCmykIcc {

inline void put32(QByteArray &b, quint32 v) { b.append(char(v >> 24)); b.append(char(v >> 16)); b.append(char(v >> 8)); b.append(char(v)); }
inline void put16(QByteArray &b, quint16 v) { b.append(char(v >> 8)); b.append(char(v)); }
inline void putSig(QByteArray &b, const char *s) { b.append(s, 4); }
inline void putS15(QByteArray &b, double v) { put32(b, quint32(qint32(std::lround(v * 65536.0)))); }
inline void putXyz(QByteArray &b, double x, double y, double z) { putS15(b, x); putS15(b, y); putS15(b, z); }
inline void pad4(QByteArray &b) { while (b.size() % 4) b.append('\0'); }

inline quint16 encL(double L) { return quint16(std::clamp(std::lround(L / 100.0 * 65280.0), 0L, 65535L)); }
inline quint16 encAB(double v) { return quint16(std::clamp(std::lround((v + 128.0) * 256.0), 0L, 65535L)); }
inline double decL(int e) { return e / 65280.0 * 100.0; }
inline double decAB(int e) { return e / 256.0 - 128.0; }

inline QByteArray lut16(int inCh, int outCh, int grid, const QVector<quint16> &clut)
{
    QByteArray t;
    putSig(t, "mft2"); put32(t, 0);
    t.append(char(inCh)); t.append(char(outCh)); t.append(char(grid)); t.append('\0');
    const double identity[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    for (double v : identity) putS15(t, v);
    put16(t, 2); put16(t, 2);
    for (int i = 0; i < inCh; ++i) { put16(t, 0); put16(t, 65535); }
    t.reserve(t.size() + clut.size() * 2 + outCh * 4);
    for (quint16 v : clut) put16(t, v);
    for (int i = 0; i < outCh; ++i) { put16(t, 0); put16(t, 65535); }
    return t;
}

inline QByteArray textDescription(const QString &text)
{
    const QByteArray ascii = text.toLatin1();
    QByteArray t;
    putSig(t, "desc"); put32(t, 0);
    put32(t, quint32(ascii.size() + 1));
    t.append(ascii); t.append('\0');
    put32(t, 0); put32(t, 0);
    put16(t, 0); t.append('\0');
    t.append(QByteArray(67, '\0'));
    return t;
}

inline QByteArray textTag(const QString &text)
{
    QByteArray t;
    putSig(t, "text"); put32(t, 0);
    t.append(text.toLatin1()); t.append('\0');
    return t;
}

inline QByteArray build(const KrimbleCmykProfile &profile, const QString &description, QWidget *parent)
{
    const KrimbleCmykEngine engine(profile);
    const KrimbleVec3 paperWhite = profile.inks.xyz[0];

    const int gA = 17;
    QVector<quint16> a2b;
    a2b.reserve(gA * gA * gA * gA * 3);
    for (int c = 0; c < gA; ++c) for (int m = 0; m < gA; ++m) for (int y = 0; y < gA; ++y) for (int k = 0; k < gA; ++k) {
        const KrimblePlates plates = {c / double(gA - 1), m / double(gA - 1), y / double(gA - 1), k / double(gA - 1)};
        const KrimbleVec3 xyz = engine.sourceXyz(plates);
        double L, a, b;
        KrimbleColorMath::xyzToLab(xyz, L, a, b);
        a2b.append(encL(L)); a2b.append(encAB(a)); a2b.append(encAB(b));
    }

    const int gB = 17;
    const int nodes = gB * gB * gB;
    QVector<quint16> b2a(nodes * 4);
    QVector<int> indices(nodes);
    for (int i = 0; i < nodes; ++i) indices[i] = i;
    auto work = [&engine, &b2a, gB](int index) {
        const int li = index / (gB * gB), ai = (index / gB) % gB, bi = index % gB;
        const double L = std::clamp(decL(li * 65535 / (gB - 1)), 0.0, 100.0);
        const double a = decAB(ai * 65535 / (gB - 1)), b = decAB(bi * 65535 / (gB - 1));
        const KrimblePlates plates = engine.separateLab(L, a, b);
        for (int i = 0; i < 4; ++i) b2a[index * 4 + i] = quint16(std::clamp(std::lround(plates[i] * 65535.0), 0L, 65535L));
    };
    QProgressDialog progress(QStringLiteral("Making the CMYK profile (the separation tables)..."), QStringLiteral("Cancel"), 0, nodes, parent);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    QFutureWatcher<void> watcher;
    QEventLoop loop;
    QObject::connect(&watcher, &QFutureWatcher<void>::finished, &loop, &QEventLoop::quit);
    QObject::connect(&watcher, &QFutureWatcher<void>::progressValueChanged, &progress, &QProgressDialog::setValue);
    QObject::connect(&progress, &QProgressDialog::canceled, &watcher, &QFutureWatcher<void>::cancel);
    watcher.setFuture(QtConcurrent::map(indices, std::function<void(int)>(work)));
    loop.exec();
    if (watcher.isCanceled()) return QByteArray();
    progress.setValue(nodes);

    const QByteArray tagA2B = lut16(4, 3, gA, a2b);
    const QByteArray tagB2A = lut16(3, 4, gB, b2a);
    QByteArray tagDesc = textDescription(description);
    QByteArray tagCprt = textTag(QStringLiteral("Made by Krimble from the Custom CMYK settings. No copyright."));
    QByteArray tagWtpt;
    putSig(tagWtpt, "XYZ "); put32(tagWtpt, 0); putXyz(tagWtpt, paperWhite.x, paperWhite.y, paperWhite.z);

    struct Tag { const char *sig; int body; };
    std::vector<QByteArray> bodies = {tagA2B, tagB2A, tagDesc, tagCprt, tagWtpt};
    const std::vector<Tag> tags = {{"A2B0", 0}, {"A2B1", 0}, {"A2B2", 0}, {"B2A0", 1}, {"B2A1", 1}, {"B2A2", 1}, {"desc", 2}, {"cprt", 3}, {"wtpt", 4}};
    const int tableSize = 4 + int(tags.size()) * 12;
    std::vector<quint32> offsets(bodies.size());
    quint32 cursor = quint32(128 + tableSize);
    for (size_t i = 0; i < bodies.size(); ++i) {
        while (cursor % 4) ++cursor;
        offsets[i] = cursor;
        cursor += quint32(bodies[i].size());
    }
    QByteArray file;
    put32(file, 0);
    put32(file, 0);
    put32(file, 0x02400000);
    putSig(file, "prtr"); putSig(file, "CMYK"); putSig(file, "Lab ");
    const QDateTime now = QDateTime::currentDateTimeUtc();
    put16(file, quint16(now.date().year())); put16(file, quint16(now.date().month())); put16(file, quint16(now.date().day()));
    put16(file, quint16(now.time().hour())); put16(file, quint16(now.time().minute())); put16(file, quint16(now.time().second()));
    putSig(file, "acsp");
    put32(file, 0); put32(file, 0); put32(file, 0); put32(file, 0);
    put32(file, 0); put32(file, 0);
    put32(file, 0);
    putXyz(file, 0.9642, 1.0, 0.8249);
    put32(file, 0);
    file.append(QByteArray(128 - file.size(), '\0'));
    put32(file, quint32(tags.size()));
    for (const Tag &t : tags) { putSig(file, t.sig); put32(file, offsets[size_t(t.body)]); put32(file, quint32(bodies[size_t(t.body)].size())); }
    for (size_t i = 0; i < bodies.size(); ++i) {
        while (quint32(file.size()) < offsets[i]) file.append('\0');
        file.append(bodies[i]);
    }
    pad4(file);
    const quint32 size = quint32(file.size());
    file[0] = char(size >> 24); file[1] = char(size >> 16); file[2] = char(size >> 8); file[3] = char(size);
    return file;
}

inline QString install(const KrimbleCmykProfile &profile, QWidget *parent)
{
    const QString description = QStringLiteral("Krimble CMYK ") + profile.displayName();
    const QByteArray data = build(profile, description, parent);
    if (data.isEmpty()) return QString();
    QString safe = description;
    for (QChar &c : safe) if (!c.isLetterOrNumber() && c != QLatin1Char(' ') && c != QLatin1Char('-') && c != QLatin1Char('(') && c != QLatin1Char(')') && c != QLatin1Char(',')) c = QLatin1Char('_');
    const QString path = KoResourcePaths::saveLocation("icc_profiles") + safe + QStringLiteral(".icc");
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(parent, QStringLiteral("Custom CMYK"), QStringLiteral("The profile file could not be written."));
        return QString();
    }
    file.write(data);
    file.close();
    KoColorSpaceEngine *icc = KoColorSpaceEngineRegistry::instance()->get("icc");
    if (!icc || !icc->addProfile(data)) {
        QMessageBox::information(parent, QStringLiteral("Custom CMYK"), QStringLiteral("The profile \"%1\" is saved. Restart Krimble to use the new version.").arg(description));
        return description;
    }
    return description;
}

}

#endif
