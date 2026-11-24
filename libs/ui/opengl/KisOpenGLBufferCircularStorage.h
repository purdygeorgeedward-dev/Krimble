/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISOPENGLBUFFERCIRCULARSTORAGE_H
#define KISOPENGLBUFFERCIRCULARSTORAGE_H

#include <QScopedPointer>
#include <QOpenGLBuffer>

class KisOpenGLBufferCircularStorage
{
public:
    struct BufferBinder
    {
        BufferBinder(KisOpenGLBufferCircularStorage *bufferStorage, const void **dataPtr, int dataSize);
        ~BufferBinder();

        BufferBinder(const BufferBinder &) = delete;
        BufferBinder &operator=(const BufferBinder &) = delete;
        BufferBinder(BufferBinder &&) = delete;
        BufferBinder &operator=(BufferBinder &&) = delete;

    private:
        QOpenGLBuffer *m_buffer = nullptr;
    };

    KisOpenGLBufferCircularStorage();
    KisOpenGLBufferCircularStorage(QOpenGLBuffer::Type type);
    ~KisOpenGLBufferCircularStorage();

    KisOpenGLBufferCircularStorage(const KisOpenGLBufferCircularStorage &) = delete;
    KisOpenGLBufferCircularStorage &operator=(const KisOpenGLBufferCircularStorage &) = delete;
    KisOpenGLBufferCircularStorage(KisOpenGLBufferCircularStorage &&) = delete;
    KisOpenGLBufferCircularStorage &operator=(KisOpenGLBufferCircularStorage &&) = delete;

    void allocate(int numBuffers, int bufferSize);
    QOpenGLBuffer* getNextBuffer();
    bool isValid() const;
    int size() const;

    void reset();

    void allocateMoreBuffers();

private:
    void addBuffersImpl(size_t buffersToAdd, int bufferSize);

    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
