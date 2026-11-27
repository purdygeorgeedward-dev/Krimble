/*
 *  SPDX-FileCopyrightText: 2005-2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "kis_png_converter.h"

#ifndef PNG_MAX_UINT
#define PNG_MAX_UINT PNG_UINT_31_MAX
#endif

#include <KoConfig.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

#include <limits.h>
#include <stdio.h>
#include <zlib.h>

#include <QBuffer>
#include <QFile>
#include <QApplication>

#include <klocalizedstring.h>
#include <QUrl>

#include <KoColorSpace.h>
#include <KoDocumentInfo.h>
#include <KoID.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorProfile.h>
#include <KoColor.h>
#include <KoUnit.h>

#include "dialogs/kis_dlg_png_import.h"
#include "kis_clipboard.h"
#include "kis_undo_stores.h"
#include <KisDocument.h>
#include <KoColorModelStandardIds.h>
#include <kis_config.h>
#include <kis_cursor_override_hijacker.h>
#include <kis_group_layer.h>
#include <kis_image.h>
#include <kis_iterator_ng.h>
#include <kis_layer.h>
#include <kis_meta_data_backend_registry.h>
#include <kis_meta_data_store.h>
#include <kis_paint_device.h>
#include <kis_paint_layer.h>
#include <kis_painter.h>
#include <kis_transaction.h>

#include <kis_assert.h>

namespace
{

int getColorTypeforColorSpace(const KoColorSpace * cs , bool alpha)
{

    QString id = cs->id();

    if (id == "GRAYA" || id == "GRAYAU16" || id == "GRAYA16") {
        return alpha ? PNG_COLOR_TYPE_GRAY_ALPHA : PNG_COLOR_TYPE_GRAY;
    }
    if (id == "RGBA" || id == "RGBA16") {
        return alpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
    }

    return -1;

}

bool colorSpaceIdSupported(const QString &id)
{
    return id == "RGBA" || id == "RGBA16" ||
        id == "GRAYA" || id == "GRAYAU16" || id == "GRAYA16";
}

QPair<QString, QString> getColorSpaceForColorType(int color_type, int color_nb_bits)
{
    QPair<QString, QString> r;

    if (color_type ==  PNG_COLOR_TYPE_PALETTE) {
        r.first = RGBAColorModelID.id();
        r.second = Integer8BitsColorDepthID.id();
    } else {
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            r.first = GrayAColorModelID.id();
        } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_RGB) {
            r.first = RGBAColorModelID.id();
        }
        if (color_nb_bits == 16) {
            r.second = Integer16BitsColorDepthID.id();
        } else if (color_nb_bits <= 8) {
            r.second = Integer8BitsColorDepthID.id();
        }
    }
    return r;
}

void fillText(png_text* p_text, const char* key, QString& text)
{
    p_text->compression = PNG_TEXT_COMPRESSION_zTXt;
    p_text->key = const_cast<char *>(key);
    char* textc = new char[text.length()+1];
    strcpy(textc, text.toLatin1());
    p_text->text = textc;
    p_text->text_length = text.length() + 1;
}

long formatStringList(char *string, const size_t length, const char *format, va_list operands)
{
    int n = vsnprintf(string, length, format, operands);

    if (n < 0)
        string[length-1] = '\0';

    return((long) n);
}

long formatString(char *string, const size_t length, const char *format, ...)
{
    long n;

    va_list operands;

    va_start(operands, format);
    n = (long) formatStringList(string, length, format, operands);
    va_end(operands);
    return(n);
}

void writeRawProfile(png_struct *ping, png_info *ping_info, QString profile_type, QByteArray profile_data)
{

    png_textp      text;

    png_uint_32    allocated_length, description_length;

    const uchar hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    dbgFile << "Writing Raw profile: type=" << profile_type << ", length=" << profile_data.length() << Qt::endl;

    text               = (png_textp) png_malloc(ping, (png_uint_32) sizeof(png_text));
    description_length = profile_type.length();
    allocated_length   = (png_uint_32)(profile_data.length() * 2 + (profile_data.length() >> 5) + 20 + description_length);

    text[0].text   = (png_charp) png_malloc(ping, allocated_length);
    memset(text[0].text, 0, allocated_length);

    QString key = QLatin1String("Raw profile type ") + profile_type.toLatin1();
    QByteArray keyData = key.toLatin1();
    text[0].key = keyData.data();

    uchar* sp = (uchar*)profile_data.data();
    png_charp dp = text[0].text;
    *dp++ = '\n';

    memcpy(dp, profile_type.toLatin1().constData(), profile_type.length());

    dp += description_length;
    *dp++ = '\n';

    formatString(dp, allocated_length - strlen(text[0].text), "%8lu ", (unsigned long)profile_data.length());

    dp += 8;

    for (long i = 0; i < (long) profile_data.length(); i++) {
        if (i % 36 == 0)
            *dp++ = '\n';

        *(dp++) = (char) hex[((*sp >> 4) & 0x0f)];
        *(dp++) = (char) hex[((*sp++) & 0x0f)];
    }

    *dp++ = '\n';
    *dp = '\0';
    text[0].text_length = (png_size_t)(dp - text[0].text);
    text[0].compression = -1;

    if (text[0].text_length <= allocated_length)
        png_set_text(ping, ping_info, text, 1);

    png_free(ping, text[0].text);
    png_free(ping, text);
}

QByteArray png_read_raw_profile(png_textp text)
{
    QByteArray profile;

    static const unsigned char unhex[103] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12,
        13, 14, 15
    };

    png_charp sp = text[0].text + 1;
    while (*sp != '\n')
        sp++;
    while (*sp == '\0' || *sp == ' ' || *sp == '\n')
        sp++;
    png_uint_32 length = (png_uint_32) atol(sp);
    while (*sp != ' ' && *sp != '\n')
        sp++;
    if (length == 0) {
        return profile;
    }
    profile.resize(length);
    unsigned char *dp = (unsigned char*)profile.data();
    png_uint_32 nibbles = length * 2;
    for (png_uint_32 i = 0; i < nibbles; i++) {
        while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f') {
            if (*sp == '\0') {
                return QByteArray();
            }
            sp++;
        }
        if (i % 2 == 0)
            *dp = (unsigned char)(16 * unhex[(int) *sp++]);
        else
            (*dp++) += unhex[(int) *sp++];
    }
    return profile;
}

void decode_meta_data(png_textp text, KisMetaData::Store* store, QString type, int headerSize)
{
    dbgFile << "Decoding " << type << " " << text[0].key;
    KisMetaData::IOBackend *exifIO = KisMetadataBackendRegistry::instance()->value(type);
    Q_ASSERT(exifIO);

    QByteArray rawProfile = png_read_raw_profile(text);
    if (headerSize > 0) {
        rawProfile.remove(0, headerSize);
    }
    if (rawProfile.size() > 0) {
        QBuffer buffer;
        buffer.setData(rawProfile);
        exifIO->loadFrom(store, &buffer);
    } else {
        dbgFile << "Decoding failed";
    }
}
}

extern "C" {
static void kis_png_warning(png_structp , png_const_charp message)
{
    qWarning("libpng warning: %s", message);
}

}

KisPNGConverter::KisPNGConverter(KisDocument *doc, bool batchMode)
{

    m_doc = doc;
    m_stop = false;
    m_max_row = 0;
    m_image = 0;
    m_batchMode = batchMode;
}

KisPNGConverter::~KisPNGConverter()
{
}

class KisPNGReadStream
{
public:
    KisPNGReadStream(quint8* buf,  quint32 depth) : m_posinc(8), m_depth(depth), m_buf(buf) {
    }
    int nextValue() {
        if (m_posinc == 0) {
            m_posinc = 8;
            m_buf++;
        }
        m_posinc -= m_depth;
        return (((*m_buf) >> (m_posinc)) & ((1 << m_depth) - 1));
    }
private:
    quint32 m_posinc, m_depth;
    quint8* m_buf;
};

class KisPNGWriteStream
{
public:
    KisPNGWriteStream(quint8* buf,  quint32 depth) : m_posinc(8), m_depth(depth), m_buf(buf) {
        *m_buf = 0;
    }
    void setNextValue(int v) {
        if (m_posinc == 0) {
            m_posinc = 8;
            m_buf++;
            *m_buf = 0;
        }
        m_posinc -= m_depth;
        *m_buf = (v << m_posinc) | *m_buf;
    }
private:
    quint32 m_posinc, m_depth;
    quint8* m_buf;
};

class KisPNGReaderAbstract
{
public:
    KisPNGReaderAbstract(png_structp _png_ptr, int _width, int _height) : png_ptr(_png_ptr), width(_width), height(_height) {}
    virtual ~KisPNGReaderAbstract() {}
    virtual png_bytep readLine() = 0;
protected:
    png_structp png_ptr;
    int width, height;
};

class KisPNGReaderLineByLine : public KisPNGReaderAbstract
{
public:
    KisPNGReaderLineByLine(png_structp _png_ptr, png_infop info_ptr, int _width, int _height) : KisPNGReaderAbstract(_png_ptr, _width, _height) {
        std::size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        row_pointer = new png_byte[rowbytes];
    }
    ~KisPNGReaderLineByLine() override {
        delete[] row_pointer;
    }
    png_bytep readLine() override {
        png_read_row(png_ptr, row_pointer, 0);
        return row_pointer;
    }
private:
    png_bytep row_pointer;
};

class KisPNGReaderFullImage : public KisPNGReaderAbstract
{
public:
    KisPNGReaderFullImage(png_structp _png_ptr, png_infop info_ptr, int _width, int _height) : KisPNGReaderAbstract(_png_ptr, _width, _height), y(0) {
        row_pointers = new png_bytep[height];
        std::size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        for (int i = 0; i < height; i++) {
            row_pointers[i] = new png_byte[rowbytes];
        }
        png_read_image(png_ptr, row_pointers);
    }
    ~KisPNGReaderFullImage() override {
        for (int i = 0; i < height; i++) {
            delete[] row_pointers[i];
        }
        delete[] row_pointers;
    }
    png_bytep readLine() override {
        return row_pointers[y++];
    }
private:
    png_bytepp row_pointers;
    int y;
};

static
void _read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    QIODevice *in = (QIODevice *)png_get_io_ptr(png_ptr);

    while (length) {
        int nr = in->read((char*)data, length);
        if (nr <= 0) {
            png_error(png_ptr, "Read Error");
            return;
        }
        length -= nr;
    }
}

static
void _write_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    QIODevice* out = (QIODevice*)png_get_io_ptr(png_ptr);

    uint nr = out->write((char*)data, length);
    if (nr != length) {
        png_error(png_ptr, "Write Error");
        return;
    }
}

static
void _flush_fn(png_structp png_ptr)
{
    Q_UNUSED(png_ptr);
}

KisImportExportErrorCode KisPNGConverter::buildImage(QIODevice* iod)
{
    dbgFile << "Start decoding PNG File";

    png_byte signature[8];
    iod->peek((char*)signature, 8);

#if PNG_LIBPNG_VER < 10400
    if (!png_check_sig(signature, 8)) {
#else
    if (png_sig_cmp(signature, 0, 8) != 0) {
#endif
        iod->close();
        return (ImportExportCodes::FileFormatIncorrect);
    }

    png_structp png_ptr =  png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (!png_ptr) {
        iod->close();
    }

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
      png_set_user_limits(png_ptr, 0x7fffffff, 0x7fffffff);
      png_set_chunk_cache_max(png_ptr, 0);
      png_set_chunk_malloc_max(png_ptr, 0);
#endif

    png_set_error_fn(png_ptr, nullptr, nullptr, kis_png_warning);
    #ifdef PNG_BENIGN_ERRORS_SUPPORTED
        png_set_benign_errors(png_ptr, 1);
    #endif

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp)0, (png_infopp)0);
        iod->close();
        return (ImportExportCodes::Failure);
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)0);
        iod->close();
        return (ImportExportCodes::Failure);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        iod->close();
        return (ImportExportCodes::Failure);
    }

    png_set_read_fn(png_ptr, iod, _read_fn);

#if defined(PNG_SKIP_sRGB_CHECK_PROFILE) && defined(PNG_SET_OPTION_SUPPORTED)
    png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);
#endif

    png_read_info(png_ptr, info_ptr);

    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY && png_get_bit_depth(png_ptr, info_ptr) < 8) {
        png_set_expand(png_ptr);
    }

    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE && png_get_bit_depth(png_ptr, info_ptr) < 8) {
        png_set_packing(png_ptr);
    }

    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE &&
            (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))) {
        png_set_expand(png_ptr);
    }
    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int color_nb_bits, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &color_nb_bits, &color_type, &interlace_type, 0, 0);
    dbgFile << "width = " << width << " height = " << height << " color_nb_bits = " << color_nb_bits << " color_type = " << color_type << " interlace_type = " << interlace_type << Qt::endl;
#ifndef WORDS_BIGENDIAN
    if (color_nb_bits > 8)
        png_set_swap(png_ptr);
#endif

    QPair<QString, QString> csName = getColorSpaceForColorType(color_type, color_nb_bits);
    if (csName.first.isEmpty()) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        iod->close();
        return ImportExportCodes::FormatColorSpaceUnsupported;
    }
    bool hasalpha = (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA);

    png_charp profile_name;
#if PNG_LIBPNG_VER_MAJOR >= 1 && PNG_LIBPNG_VER_MINOR >= 5
    png_bytep profile_data;
#else
    png_charp profile_data;
#endif
    int compression_type;
    png_uint_32 proflen;

#if defined(PNG_cHRM_SUPPORTED)
    double whitePointX, whitePointY;
    double redX, redY;
    double greenX, greenY;
    double blueX, blueY;
    png_get_cHRM(png_ptr,info_ptr, &whitePointX, &whitePointY, &redX, &redY, &greenX, &greenY, &blueX, &blueY);
    dbgFile << "cHRM:" << whitePointX << whitePointY << redX << redY << greenX << greenY << blueX << blueY;
#endif

#if defined(PNG_GAMMA_SUPPORTED)
    double gamma;
    png_get_gAMA(png_ptr, info_ptr, &gamma);
    dbgFile << "gAMA" << gamma;
#endif

#if defined(PNG_sRGB_SUPPORTED)
    int sRGBIntent;
    png_get_sRGB(png_ptr, info_ptr, &sRGBIntent);
    dbgFile << "sRGB" << sRGBIntent;
#endif

    bool fromBlender = false;

    png_text* text_ptr;
    int num_comments;
    png_get_text(png_ptr, info_ptr, &text_ptr, &num_comments);

    for (int i = 0; i < num_comments; i++) {
        QString key = QString(text_ptr[i].key).toLower();
        if (key == "file") {
            QString relatedFile = text_ptr[i].text;
            if (relatedFile.contains(".blend", Qt::CaseInsensitive)){
                fromBlender=true;
            }
        }
    }

    bool loadedImageIsHDR = false;
    const KoColorProfile* profile = KoColorSpaceRegistry::instance()->p709SRGBProfile();

    if (png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &proflen)) {
        QByteArray profile_rawdata(reinterpret_cast<char*>(profile_data), proflen);
        profile = KoColorSpaceRegistry::instance()->createColorProfile(csName.first, csName.second, profile_rawdata);
        if (profile) {
            if (!profile->isSuitableForWorkspace()) {
                dbgFile << "the profile is not suitable for output and therefore cannot be used in krita, we need to convert the image to a standard profile";
            }
        }

        loadedImageIsHDR = strcmp(profile_name, "ITUR_2100_PQ_FULL") == 0;
    }
    else if (color_nb_bits == 16 && !fromBlender && !qAppName().toLower().contains("test") && !m_batchMode) {
        KisConfig cfg(true);
        quint32 behaviour = cfg.pasteBehaviour();
        if (behaviour == KisClipboard::PASTE_ASK) {
            KisDlgPngImport dlg(m_path, csName.first, csName.second);
            KisCursorOverrideHijacker hijacker;
            Q_UNUSED(hijacker);
            dlg.exec();
            if (!dlg.profile().isEmpty()) {
                profile = KoColorSpaceRegistry::instance()->profileByName(dlg.profile());
            }
        }
    }
    else {
        dbgFile << "no embedded profile, will use the default sRGB profile";
    }

    const QString colorSpaceId =
        KoColorSpaceRegistry::instance()->colorSpaceId(csName.first, csName.second);

    if (profile
        && (!KoColorSpaceRegistry::instance()->profileIsCompatible(profile, colorSpaceId)
        ||  !(profile->isSuitableForOutput() || profile->isSuitableForInput()))) {
        warnFile << "The profile " << profile->name() << " is not compatible with the color space model " << csName.first << " " << csName.second;
        profile = 0;
    }

    KoColorConversionTransformation* transform = 0;
    const KoColorSpace* cs = 0;

    if (loadedImageIsHDR &&
        csName.first == RGBAColorModelID.id() &&
        csName.second == Integer16BitsColorDepthID.id()) {

        const KoColorSpace *p2020PQCS =
            KoColorSpaceRegistry::instance()->colorSpace(
                RGBAColorModelID.id(),
                Integer16BitsColorDepthID.id(),
                KoColorSpaceRegistry::instance()->p2020PQProfile());

        cs = p2020PQCS;

    } else if (profile && profile->isSuitableForWorkspace()) {
        dbgFile << "image has embedded profile: " << profile->name() << "\n";
        cs = KoColorSpaceRegistry::instance()->colorSpace(csName.first, csName.second, profile);
    }
    else {
        cs = KoColorSpaceRegistry::instance()->colorSpace(csName.first, csName.second, "");
    }

    if (profile  && !profile->isSuitableForWorkspace() && profile->isSuitableForInput()) {
      transform = KoColorSpaceRegistry::instance()->colorSpace(csName.first, csName.second, profile)->createColorConverter(cs, KoColorConversionTransformation::internalRenderingIntent(), KoColorConversionTransformation::internalConversionFlags());
    }

    if (cs == 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return ImportExportCodes::FormatColorSpaceUnsupported;
    }

    if (m_image == 0) {
        KisUndoStore *store = m_doc ? m_doc->createUndoStore() : new KisSurrogateUndoStore();
        m_image = new KisImage(store, width, height, cs, "built image");
    }

    int unit_type;
    png_uint_32 x_resolution = 0, y_resolution = 0;

    png_get_pHYs(png_ptr, info_ptr, &x_resolution, &y_resolution, &unit_type);
    if (x_resolution > 0 && y_resolution > 0 && unit_type == PNG_RESOLUTION_METER) {
        m_image->setResolution((double) POINT_TO_CM(x_resolution) / 100.0, (double) POINT_TO_CM(y_resolution) / 100.0);
    } else if (unit_type == PNG_RESOLUTION_UNKNOWN) {
        m_image->setResolution(100.0, 100.0);
    }

    double coeff = quint8_MAX / (double)(pow((double)2, color_nb_bits) - 1);
    KisPaintLayerSP layer = new KisPaintLayer(m_image.data(), m_image -> nextLayerName(), UCHAR_MAX);

    png_get_text(png_ptr, info_ptr, &text_ptr, &num_comments);
    if (m_doc) {
        KoDocumentInfo * info = m_doc->documentInfo();
        dbgFile << "There are " << num_comments << " comments in the text";
        for (int i = 0; i < num_comments; i++) {
            QString key = QString(text_ptr[i].key).toLower();
            dbgFile << "key: " << text_ptr[i].key
                    << ", containing: " << text_ptr[i].text
                    << ": " << (key == "raw profile type exif " ? "isExif" : "something else");
            if (key == "title") {
                info->setAboutInfo("title", text_ptr[i].text);
            } else if (key == "description") {
                info->setAboutInfo("comment", text_ptr[i].text);
            } else if (key == "author") {
                info->setAuthorInfo("creator", text_ptr[i].text);
            } else if (key.contains("raw profile type exif")) {
                decode_meta_data(text_ptr + i, layer->metaData(), "exif", 6);
            } else if (key.contains("raw profile type iptc")) {
                decode_meta_data(text_ptr + i, layer->metaData(), "iptc", 14);
            } else if (key.contains("raw profile type xmp")) {
                decode_meta_data(text_ptr + i, layer->metaData(), "xmp", 0);
            } else if (key == "version") {
                m_image->addAnnotation(new KisAnnotation("kpp_version", "version", QByteArray(text_ptr[i].text)));
            } else if (key == "preset") {
                m_image->addAnnotation(new KisAnnotation("kpp_preset", "preset", QByteArray(text_ptr[i].text)));
            }
        }
    }
    QScopedPointer<KisPNGReaderAbstract> reader;
    try {
        if (interlace_type == PNG_INTERLACE_ADAM7) {
            reader.reset(new KisPNGReaderFullImage(png_ptr, info_ptr, width, height));
        } else {
            reader.reset(new KisPNGReaderLineByLine(png_ptr, info_ptr, width, height));
        }
    } catch (const std::bad_alloc& e) {
        dbgFile << "bad alloc: " << e.what();
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return (ImportExportCodes::Failure);
    }

    png_colorp palette ;
    int num_palette;
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    }

    quint8 palette_alpha[256];
    memset(palette_alpha, 255, 256);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_bytep alpha_ptr;
            int num_alpha;
            png_get_tRNS(png_ptr, info_ptr, &alpha_ptr, &num_alpha, 0);
            for (int i = 0; i < num_alpha; ++i) {
                palette_alpha[i] = alpha_ptr[i];
            }
        }
    }

    for (png_uint_32 y = 0; y < height; y++) {
        KisHLineIteratorSP it = layer -> paintDevice() -> createHLineIteratorNG(0, y, width);

        png_bytep row_pointer = reader->readLine();

        switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (color_nb_bits == 16) {
                quint16 *src = reinterpret_cast<quint16 *>(row_pointer);
                do {
                    quint16 *d = reinterpret_cast<quint16 *>(it->rawData());
                    d[0] = *(src++);
                    if (hasalpha) {
                        d[1] = *(src++);
                    } else {
                        d[1] = quint16_MAX;
                    }
                    if (transform) transform->transformInPlace(reinterpret_cast<quint8*>(d), reinterpret_cast<quint8*>(d), 1);
                } while (it->nextPixel());
            } else  {
                KisPNGReadStream stream(row_pointer, color_nb_bits);
                do {
                    quint8 *d = it->rawData();
                    d[0] = (quint8)(stream.nextValue() * coeff);
                    if (hasalpha) {
                        d[1] = (quint8)(stream.nextValue() * coeff);
                    } else {
                        d[1] = UCHAR_MAX;
                    }
                    if (transform) transform->transformInPlace(d, d, 1);
                } while (it->nextPixel());
            }
            break;
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (color_nb_bits == 16) {
                quint16 *src = reinterpret_cast<quint16 *>(row_pointer);
                do {
                    quint16 *d = reinterpret_cast<quint16 *>(it->rawData());
                    d[2] = *(src++);
                    d[1] = *(src++);
                    d[0] = *(src++);
                    if (hasalpha) d[3] = *(src++);
                    else d[3] = quint16_MAX;
                    if (transform) transform->transformInPlace(reinterpret_cast<quint8 *>(d), reinterpret_cast<quint8*>(d), 1);
                } while (it->nextPixel());
            } else {
                KisPNGReadStream stream(row_pointer, color_nb_bits);
                do {
                    quint8 *d = it->rawData();
                    d[2] = (quint8)(stream.nextValue() * coeff);
                    d[1] = (quint8)(stream.nextValue() * coeff);
                    d[0] = (quint8)(stream.nextValue() * coeff);
                    if (hasalpha) d[3] = (quint8)(stream.nextValue() * coeff);
                    else d[3] = UCHAR_MAX;
                    if (transform) transform->transformInPlace(d, d, 1);
                } while (it->nextPixel());
            }
            break;
        case PNG_COLOR_TYPE_PALETTE: {
            KisPNGReadStream stream(row_pointer, color_nb_bits);
            do {
                quint8 *d = it->rawData();
                quint8 index = stream.nextValue();
                quint8 alpha = palette_alpha[ index ];
                if (alpha == 0) {
                    memset(d, 0, 4);
                } else {
                    png_color c = palette[ index ];
                    d[2] = c.red;
                    d[1] = c.green;
                    d[0] = c.blue;
                    d[3] = alpha;
                }
            } while (it->nextPixel());
        }
            break;
        default:
            return ImportExportCodes::FormatFeaturesUnsupported;
        }
    }
    m_image->addNode(layer.data(), m_image->rootLayer().data());

    png_read_end(png_ptr, end_info);
    iod->close();

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return ImportExportCodes::OK;

}

KisImportExportErrorCode KisPNGConverter::buildImage(const QString &filename)
{
    m_path = filename;

    QFile fp(filename);
    if (fp.exists()) {
        if (!fp.open(QIODevice::ReadOnly)) {
            dbgFile << "Failed to open PNG File";
            return (ImportExportCodes::FileFormatIncorrect);
        }

        return buildImage(&fp);
    }
    return (ImportExportCodes::FileNotExist);

}

KisImageSP KisPNGConverter::image()
{
    return m_image;
}

bool KisPNGConverter::saveDeviceToStore(const QString &filename, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP dev, KoStore *store, KisMetaData::Store* metaData)
{
    if (store->open(filename)) {
        KoStoreDevice io(store);
        if (!io.open(QIODevice::WriteOnly)) {
            dbgFile << "Could not open for writing:" << filename;
            return false;
        }
        KisPNGConverter pngconv(0);
        vKisAnnotationSP_it annotIt;
        KisMetaData::Store* metaDataStore = 0;
        if (metaData) {
            metaDataStore = new KisMetaData::Store(*metaData);
        }
        KisPNGOptions options;
        options.compression = 3;
        options.interlace = false;
        options.tryToSaveAsIndexed = false;
        options.alpha = true;
        options.saveSRGBProfile = false;
        options.downsample = false;

        if (dev->colorSpace()->id() != "RGBA") {
            dev = new KisPaintDevice(*dev.data());
            dev->convertTo(KoColorSpaceRegistry::instance()->rgb8());
        }

        KisImportExportErrorCode success = pngconv.buildFile(&io, imageRect, xRes, yRes, dev, annotIt, annotIt, options, metaDataStore);
        if (!success.isOk()) {
            dbgFile << "Saving PNG failed:" << filename;
            delete metaDataStore;
            return false;
        }
        delete metaDataStore;
        io.close();
        if (!store->close()) {
            return false;
        }
    } else {
        dbgFile << "Opening of data file failed :" << filename;
        return false;
    }
    return true;

}

KisImportExportErrorCode KisPNGConverter::buildFile(const QString &filename, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP device, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, KisPNGOptions options, KisMetaData::Store* metaData)
{
    dbgFile << "Start writing PNG File " << filename;
    QFile fp (filename);
    if (!fp.open(QIODevice::WriteOnly)) {
        dbgFile << "Failed to open PNG File for writing";
        return (KisImportExportErrorCannotWrite(fp.error()));
    }

    KisImportExportErrorCode result = buildFile(&fp, imageRect, xRes, yRes, device, annotationsStart, annotationsEnd, options, metaData);

    return result;
}

KisImportExportErrorCode KisPNGConverter::buildFile(QIODevice* iodevice, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP device, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, KisPNGOptions options, KisMetaData::Store* metaData)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(device, ImportExportCodes::InternalError);

    if (!options.alpha) {
        KisPaintDeviceSP tmp = new KisPaintDevice(device->colorSpace());
        KoColor c(options.transparencyFillColor, device->colorSpace());
        tmp->fill(imageRect, c);
        KisPainter gc(tmp);
        gc.bitBlt(imageRect.topLeft(), device, imageRect);
        gc.end();
        device = tmp;
    }

    KIS_SAFE_ASSERT_RECOVER(!options.saveAsHDR || !options.forceSRGB) {
        options.forceSRGB = false;
    }

    QStringList colormodels = QStringList() << RGBAColorModelID.id() << GrayAColorModelID.id();
    QString dstModel = device->colorSpace()->colorModelId().id();
    QString dstDepth = device->colorSpace()->colorDepthId().id();
    const KoColorProfile *dstProfile = device->colorSpace()->profile();
    bool needColorTransform = false;

    if (options.saveAsHDR || options.forceSRGB || !colormodels.contains(device->colorSpace()->colorModelId().id())) {
        dstModel = RGBAColorModelID.id();
        dstProfile = KoColorSpaceRegistry::instance()->p709SRGBProfile();

        needColorTransform = true;

        if (options.saveAsHDR) {
            dstProfile = KoColorSpaceRegistry::instance()->p2020PQProfile();
        }
    }

    if (options.downsample
            || device->colorSpace()->colorDepthId() == Float16BitsColorDepthID
            || device->colorSpace()->colorDepthId() == Float32BitsColorDepthID
            || device->colorSpace()->colorDepthId() == Float64BitsColorDepthID) {
        dstDepth = Integer16BitsColorDepthID.id();

        needColorTransform = true;

        if (options.downsample) {
            dstDepth = Integer8BitsColorDepthID.id();
        }
    }

    if (needColorTransform) {
        const KoColorSpace *dstCs = KoColorSpaceRegistry::instance()->colorSpace(dstModel, dstDepth, dstProfile);

        if (!dstCs) {
            return ImportExportCodes::FormatColorSpaceUnsupported;
        }

        device = new KisPaintDevice(*device);
        device->convertTo(dstCs);
    }

    KIS_SAFE_ASSERT_RECOVER(!options.saveAsHDR || !options.tryToSaveAsIndexed) {
        options.tryToSaveAsIndexed = false;
    }

    png_structp png_ptr =  png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) {
        return (ImportExportCodes::Failure);
    }

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
      png_set_user_limits(png_ptr, 0x7fffffff, 0x7fffffff);
      png_set_chunk_cache_max(png_ptr, 0);
      png_set_chunk_malloc_max(png_ptr, 0);
#endif

    png_set_error_fn(png_ptr, nullptr, nullptr, kis_png_warning);
    #ifdef PNG_BENIGN_ERRORS_SUPPORTED
        png_set_benign_errors(png_ptr, 1);
    #endif

#if defined(PNG_SKIP_sRGB_CHECK_PROFILE) && defined(PNG_SET_OPTION_SUPPORTED)
    png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);
#endif

#ifdef PNG_READ_CHECK_FOR_INVALID_INDEX_SUPPORTED
    png_set_check_for_invalid_index(png_ptr, 0);
#endif

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)0);
        return (ImportExportCodes::Failure);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return (ImportExportCodes::Failure);
    }

    png_set_compression_level(png_ptr, options.compression);

    png_set_write_fn(png_ptr, (void*)iodevice, _write_fn, _flush_fn);

    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
    png_set_compression_buffer_size(png_ptr, 8192);

    int color_nb_bits = 8 * device->pixelSize() / device->channelCount();
    int color_type = getColorTypeforColorSpace(device->colorSpace(), options.alpha);

    Q_ASSERT(color_type > -1);

    QScopedArrayPointer<png_color> palette;
    int num_palette = 0;
    if (!options.alpha && options.tryToSaveAsIndexed && KoID(device->colorSpace()->id()) == KoID("RGBA")) {
        palette.reset(new png_color[255]);

        KisSequentialIterator it(device, imageRect);

        bool toomuchcolor = false;
        while (it.nextPixel()) {
            const quint8* c = it.oldRawData();
            bool findit = false;
            for (int i = 0; i < num_palette; i++) {
                if (palette[i].red == c[2] &&
                        palette[i].green == c[1] &&
                        palette[i].blue == c[0]) {
                    findit = true;
                    break;
                }
            }
            if (!findit) {
                if (num_palette == 255) {
                    toomuchcolor = true;
                    break;
                }
                palette[num_palette].red = c[2];
                palette[num_palette].green = c[1];
                palette[num_palette].blue = c[0];
                num_palette++;
            }
        }

        if (!toomuchcolor) {
            dbgFile << "Found a palette of " << num_palette << " colors";
            color_type = PNG_COLOR_TYPE_PALETTE;
            if (num_palette <= 2) {
                color_nb_bits = 1;
            } else if (num_palette <= 4) {
                color_nb_bits = 2;
            } else if (num_palette <= 16) {
                color_nb_bits = 4;
            } else {
                color_nb_bits = 8;
            }
        } else {
            palette.reset();
        }
    }

    int interlace_type = options.interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(color_type >= 0, ImportExportCodes::Failure);

    png_set_IHDR(png_ptr, info_ptr,
                 imageRect.width(),
                 imageRect.height(),
                 color_nb_bits,
                 color_type, interlace_type,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    const bool sRGB = *device->colorSpace()->profile() == *KoColorSpaceRegistry::instance()->p709SRGBProfile();

#if 0
    if (options.saveAsHDR) {
#if defined(PNG_GAMMA_SUPPORTED)

        png_set_gAMA_fixed(png_ptr, info_ptr, 15000);
        dbgFile << "gAMA" << "(Rec 2100)";
#endif

#if defined PNG_cHRM_SUPPORTED
        png_set_cHRM_fixed(png_ptr, info_ptr,
                           31270, 32900,
                           70800, 29200,
                           17000, 79700,
                           13100, 4600
                           );
        dbgFile << "cHRM" << "(Rec 2100)";
#endif
    }
#endif

    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(palette || color_type != PNG_COLOR_TYPE_PALETTE, ImportExportCodes::Failure);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_PLTE(png_ptr, info_ptr, palette.data(), num_palette);
    }
    vKisAnnotationSP_it it = annotationsStart;
    while (it != annotationsEnd) {
        if (!(*it) || (*it)->type().isEmpty()) {
            dbgFile << "Warning: empty annotation";
            it++;
            continue;
        }

        dbgFile << "Trying to store annotation of type " << (*it) -> type() << " of size " << (*it) -> annotation() . size();

        if ((*it) -> type().startsWith(QString("krita_attribute:"))) {
            dbgFile << "cannot save this annotation : " << (*it) -> type();
        } else if ((*it)->type() == "kpp_version" || (*it)->type() == "kpp_preset" ) {
            dbgFile << "Saving preset information " << (*it)->description();
            png_textp      text = (png_textp) png_malloc(png_ptr, (png_uint_32) sizeof(png_text));

            QByteArray keyData = (*it)->description().toLatin1();
            text[0].key = keyData.data();
            text[0].text = (char*)(*it)->annotation().data();
            text[0].text_length = (*it)->annotation().size();
            text[0].compression = -1;

            png_set_text(png_ptr, info_ptr, text, 1);
            png_free(png_ptr, text);
        }
        it++;
    }

    const KoColorProfile* colorProfile = device->colorSpace()->profile();
    QByteArray colorProfileData = colorProfile->rawData();
    if (!sRGB || options.saveSRGBProfile) {

#if PNG_LIBPNG_VER_MAJOR >= 1 && PNG_LIBPNG_VER_MINOR >= 5
        const char *typeString = !options.saveAsHDR ? "icc" : "ITUR_2100_PQ_FULL";
        png_set_iCCP(png_ptr, info_ptr, (png_const_charp)typeString, PNG_COMPRESSION_TYPE_BASE, (png_const_bytep)colorProfileData.constData(), colorProfileData . size());
#else
        char typeStringICC[] = "icc";
        char typeStringHDR[] = "ITUR_2100_PQ_FULL";
        char *typeString = !options.saveAsHDR ? typeStringICC : typeStringHDR;
        png_set_iCCP(png_ptr, info_ptr, typeString, PNG_COMPRESSION_TYPE_BASE, colorProfileData.data(), colorProfileData . size());
#endif
    }

    if (m_doc) {
        png_text texts[4];
        int nbtexts = 0;
        KoDocumentInfo * info = m_doc->documentInfo();
        QString title = info->aboutInfo("title");
        if (!title.isEmpty() && options.storeMetaData) {
            fillText(texts + nbtexts, "Title", title);
            nbtexts++;
        }
        QString abstract = info->aboutInfo("subject");
        if (abstract.isEmpty()) {
            abstract = info->aboutInfo("abstract");
        }
        if (!abstract.isEmpty() && options.storeMetaData) {
            QString keywords = info->aboutInfo("keyword");
            if (!keywords.isEmpty()) {
                abstract = abstract + " keywords: " + keywords;
            }
            fillText(texts + nbtexts, "Description", abstract);
            nbtexts++;
        }

        QString license = info->aboutInfo("license");
        if (!license.isEmpty() && options.storeMetaData) {
            fillText(texts + nbtexts, "Copyright", license);
            nbtexts++;
        }

        QString author = info->authorInfo("creator");
        if (!author.isEmpty() && options.storeAuthor) {
            if (!info->authorContactInfo().isEmpty()) {
                QString contact = info->authorContactInfo().at(0);
                if (!contact.isEmpty()) {
                    author = author+"("+contact+")";
                }
            }
            fillText(texts + nbtexts, "Author", author);
            nbtexts++;
        }

        png_set_text(png_ptr, info_ptr, texts, nbtexts);
    }

    if (metaData && !metaData->empty()) {
        if (options.exif) {
            dbgFile << "Trying to save exif information";

            KisMetaData::IOBackend *exifIO = KisMetadataBackendRegistry::instance()->value("exif");
            Q_ASSERT(exifIO);

            QBuffer buffer;
            exifIO->saveTo(metaData, &buffer, KisMetaData::IOBackend::JpegHeader);
            writeRawProfile(png_ptr, info_ptr, "exif", buffer.data());
        }
        if (options.iptc) {
            dbgFile << "Trying to save iptc information";
            KisMetaData::IOBackend *iptcIO = KisMetadataBackendRegistry::instance()->value("iptc");
            Q_ASSERT(iptcIO);

            QBuffer buffer;
            iptcIO->saveTo(metaData, &buffer, KisMetaData::IOBackend::JpegHeader);

            dbgFile << "IPTC information size is" << buffer.data().size();
            writeRawProfile(png_ptr, info_ptr, "iptc", buffer.data());
        }
        if (options.xmp) {
            dbgFile << "Trying to save XMP information";
            KisMetaData::IOBackend *xmpIO = KisMetadataBackendRegistry::instance()->value("xmp");
            Q_ASSERT(xmpIO);

            QBuffer buffer;
            xmpIO->saveTo(metaData, &buffer, KisMetaData::IOBackend::NoHeader);

            dbgFile << "XMP information size is" << buffer.data().size();
            writeRawProfile(png_ptr, info_ptr, "xmp", buffer.data());
        }
    }
#if 0
    int unit_type;
    png_uint_32 x_resolution, y_resolution;
#endif
    png_set_pHYs(png_ptr, info_ptr, CM_TO_POINT(xRes) * 100.0, CM_TO_POINT(yRes) * 100.0, PNG_RESOLUTION_METER);

    png_write_info(png_ptr, info_ptr);
    png_write_flush(png_ptr);

#ifndef WORDS_BIGENDIAN
    if (color_nb_bits > 8)
        png_set_swap(png_ptr);
#endif

    struct RowPointersStruct {
        RowPointersStruct(const QSize &size, int pixelSize)
            : numRows(size.height())
        {
            rows = new png_byte*[numRows];

            for (int i = 0; i < numRows; i++) {
                rows[i] = new png_byte[size.width() * pixelSize];
            }
        }

        ~RowPointersStruct() {
            for (int i = 0; i < numRows; i++) {
                delete[] rows[i];
            }
            delete[] rows;
        }

        const int numRows = 0;
        png_byte** rows = 0;
    };

    RowPointersStruct rowPointers(imageRect.size(), device->pixelSize());

    int row = 0;
    for (int y = imageRect.y(); y < imageRect.y() + imageRect.height(); y++, row++) {
        KisHLineConstIteratorSP it = device->createHLineConstIteratorNG(imageRect.x(), y, imageRect.width());

        switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (color_nb_bits == 16) {
                quint16 *dst = reinterpret_cast<quint16 *>(rowPointers.rows[row]);
                do {
                    const quint16 *d = reinterpret_cast<const quint16 *>(it->oldRawData());
                    *(dst++) = d[0];
                    if (options.alpha) *(dst++) = d[1];
                } while (it->nextPixel());
            } else {
                quint8 *dst = rowPointers.rows[row];
                do {
                    const quint8 *d = it->oldRawData();
                    *(dst++) = d[0];
                    if (options.alpha) *(dst++) = d[1];
                } while (it->nextPixel());
            }
            break;
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (color_nb_bits == 16) {
                quint16 *dst = reinterpret_cast<quint16 *>(rowPointers.rows[row]);
                do {
                    const quint16 *d = reinterpret_cast<const quint16 *>(it->oldRawData());
                    *(dst++) = d[2];
                    *(dst++) = d[1];
                    *(dst++) = d[0];
                    if (options.alpha) *(dst++) = d[3];
                } while (it->nextPixel());
            } else {
                quint8 *dst = rowPointers.rows[row];
                do {
                    const quint8 *d = it->oldRawData();
                    *(dst++) = d[2];
                    *(dst++) = d[1];
                    *(dst++) = d[0];
                    if (options.alpha) *(dst++) = d[3];
                } while (it->nextPixel());
            }
            break;
        case PNG_COLOR_TYPE_PALETTE: {
            quint8 *dst = rowPointers.rows[row];
            KisPNGWriteStream writestream(dst, color_nb_bits);
            do {
                const quint8 *d = it->oldRawData();
                int i;
                for (i = 0; i < num_palette; i++) {
                    if (palette[i].red == d[2] &&
                            palette[i].green == d[1] &&
                            palette[i].blue == d[0]) {
                        break;
                    }
                }
                writestream.setNextValue(i);
            } while (it->nextPixel());
        }
            break;
        default:
            return ImportExportCodes::FormatColorSpaceUnsupported;
        }
    }

    png_write_image(png_ptr, rowPointers.rows);

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return ImportExportCodes::OK;
}

void KisPNGConverter::cancel()
{
    m_stop = true;
}

void KisPNGConverter::progress(png_structp png_ptr, png_uint_32 row_number, int pass)
{
    if (png_ptr == 0 || row_number > PNG_MAX_UINT || pass > 7) return;
}

bool KisPNGConverter::isColorSpaceSupported(const KoColorSpace *cs)
{
    return colorSpaceIdSupported(cs->id());
}

