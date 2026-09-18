#include "logging.h"

#include <QByteArray>
#include <QMetaType>
#include <QVariant>
#include <QDateTime>
#include <QStringBuilder>

static QString escape(const QString &string);
static QString toString(const QVariantMap &map);
static QString toString(const QVariantList &list);
static QString toString(const QVariantHash &hash);
static QString toString(const QStringList &stringList);
static QString toString(const QByteArray &byteArray);
static QString toString(QDate date);
static QString toString(QTime time);
static QString toString(const QDateTime &datetime);
static QString toString(const QString &string);
static QString toString(const QVariant &value, int userType);
static QString toString(const QVariant &value);
static bool isOpenBracket(const QStringList &brackets, QChar c);
static bool isCloseBracket(const QStringList &brackets, QChar c);

QString Logging::createDumpString(const QByteArray &data, const bool withCaption)
{
    const int bytesPerLine  = 16;
    const QByteArray dist("  ");
    const QByteArray placeHolder("   ");
    const QByteArray intention("      ");
    QByteArray line;
    QByteArray details(dist);
    unsigned char fromChar = 0x20;
    unsigned char toChar   = 0x7e;
    QByteArray out;
    if (withCaption)
        out.append("Dump:\n      offset (hex) - data (hex) - data (text)\n");

    QByteArray hexData = data.toHex();

    line.append(intention);
    line.append("00000000  ");
    for ( int i = 1, ii = 0; i <= data.size(); ++i, ii += 2 )
    {
        unsigned char byte = data[i - 1];

        line.append(hexData.mid(ii, 2));
        line.append(' ');

        if ( byte >= fromChar && byte <= toChar )
            details.append(static_cast<char>(byte));
        else
            details.append('.');

        if (( i % bytesPerLine ) == 0)
        {
            // one line ready
            line.append(details);
            out.append(line);
            out.append('\n');
            line.clear();

            line.append(intention);
            line.append(QStringLiteral("%0  ").arg(i, 8, 16, QLatin1Char('0')).toLatin1());

            details.clear();
            details.append(dist);
        }
        else if (i == data.size())
        {
            int remaining = bytesPerLine - ( i % bytesPerLine );
            for ( int j = 0; j < remaining; j++ )
                line.append(placeHolder);

            // end if data
            line.append(details);
            out.append(line);
        }
    }
    return out;
}

QString Logging::toString(const QVariant &value)
{
    return ::toString(value);
}

QString toString(const QVariant &value)
{
    const int typeId = value.typeId();

    // In Qt 6 a user type is no longer a single enum value, it is every id at
    // or above QMetaType::User, so it cannot be written as a case label.
    if (typeId >= QMetaType::User)
        return toString(value, value.userType());

    switch (typeId)
    {
    case QMetaType::UnknownType:
        return QStringLiteral("<Invalid>");
    case QMetaType::QBitArray:
        return QStringLiteral("<BitArray>");
    case QMetaType::QBitmap:
        return QStringLiteral("<Bitmap>");
    case QMetaType::QBrush:
        return QStringLiteral("<Brush>");
    case QMetaType::QColor:
        return QStringLiteral("<Color>");
    case QMetaType::QCursor:
        return QStringLiteral("<Cursor>");
    case QMetaType::QEasingCurve:
        return QStringLiteral("<EasingCurve>");
    case QMetaType::QModelIndex:
        return QStringLiteral("<ModelIndex>");
    case QMetaType::QFont:
        return QStringLiteral("<Font>");
    case QMetaType::QIcon:
        return QStringLiteral("<Icon>");
    case QMetaType::QImage:
        return QStringLiteral("<Image>");
    case QMetaType::QKeySequence:
        return QStringLiteral("<KeySequence>");
    case QMetaType::QLine:
        return QStringLiteral("<Line>");
    case QMetaType::QLineF:
        return QStringLiteral("<LineF>");
    case QMetaType::QLocale:
        return QStringLiteral("<Locale>");
#if QT_VERSION < 0x060000
    case QVariant::Matrix:
        return QStringLiteral("<Matrix>");
#endif
    case QMetaType::QTransform:
        return QStringLiteral("<Transform>");
    case QMetaType::QMatrix4x4:
        return QStringLiteral("<Matrix4x4>");
    case QMetaType::QPalette:
        return QStringLiteral("<Palette>");
    case QMetaType::QPen:
        return QStringLiteral("<Pen>");
    case QMetaType::QPixmap:
        return QStringLiteral("<Pixmap>");
    case QMetaType::QPoint:
        return QStringLiteral("<Point>");
    case QMetaType::QPointF:
        return QStringLiteral("<PointF>");
    case QMetaType::QPolygon:
        return QStringLiteral("<Polygon>");
    case QMetaType::QPolygonF:
        return QStringLiteral("<PolygonF>");
    case QMetaType::QQuaternion:
        return QStringLiteral("<Quaternion>");
    case QMetaType::QRect:
        return QStringLiteral("<Rect>");
    case QMetaType::QRectF:
        return QStringLiteral("<RectF>");
#if QT_VERSION < 0x060000
    case QVariant::RegExp:
        return QStringLiteral("<RegExp>");
#endif
    case QMetaType::QRegularExpression:
        return QStringLiteral("<RegularExpression>");
    case QMetaType::QRegion:
        return QStringLiteral("<Region>");
    case QMetaType::QSize:
        return QStringLiteral("<Size>");
    case QMetaType::QSizeF:
        return QStringLiteral("<SizeF>");
    case QMetaType::QSizePolicy:
        return QStringLiteral("<SizePolicy>");
    case QMetaType::QTextFormat:
        return QStringLiteral("<TextFormat>");
    case QMetaType::QTextLength:
        return QStringLiteral("<TextLength>");
    case QMetaType::QVector2D:
        return QStringLiteral("<Vector2D>");
    case QMetaType::QVector3D:
        return QStringLiteral("<Vector3D>");
    case QMetaType::QVector4D:
        return QStringLiteral("<Vector4D>");

    case QMetaType::Int:
    case QMetaType::Double:
    case QMetaType::QChar:
    case QMetaType::Bool:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::QUrl:
    case QMetaType::QUuid:
        return value.toString();

    case QMetaType::QByteArray:
        return toString(value.toByteArray());

    case QMetaType::QDate:
        return toString(value.toDate());

    case QMetaType::QDateTime:
        return toString(value.toDateTime());

    case QMetaType::QVariantHash:
        return toString(value.toHash());

    case QMetaType::QVariantList:
        return toString(value.toList());

    case QMetaType::QVariantMap:
        return toString(value.toMap());

    case QMetaType::QString:
        return toString(value.toString());

    case QMetaType::QStringList:
        return toString(value.toStringList());

    case QMetaType::QTime:
        return toString(value.toTime());

    default:
        break;
    }
    return QStringLiteral("<Unknow variant type>");
}

QString toString(const QVariantMap &map)
{
    QStringList result;
    for (auto pos = map.cbegin(); pos != map.cend(); ++pos)
        result << QStringLiteral("%1=%2").arg(toString(pos.key()), toString(pos.value()));
    return QStringLiteral("{") % result.join(QStringLiteral(", ")) % QStringLiteral("}");
}

QString toString(const QVariantList &list)
{
    QStringList result;
    for (const auto &value : list)
        result << toString(value);
    return QStringLiteral("[") % result.join(QStringLiteral(", ")) % QStringLiteral("]");
}

QString toString(const QVariantHash &hash)
{
    QStringList result;
    for (auto pos = hash.cbegin(); pos != hash.cend(); ++pos)
        result << QStringLiteral("%1=%2").arg(toString(pos.key()), toString(pos.value()));
    return QStringLiteral("{") % result.join(QStringLiteral(", ")) % QStringLiteral("}");
}

QString toString(const QStringList &stringList)
{
    QStringList result;
    for (const auto &string : stringList)
        result << toString(string);
    return QStringLiteral("[") % result.join(QStringLiteral(", ")) % QStringLiteral("]");
}

QString toString(const QByteArray &byteArray)
{
    QStringList result;
    for (auto byte : byteArray)
        result << QStringLiteral("%1").arg(byte, 2, 16, QChar('0'));
    return QStringLiteral("[") % result.join(QStringLiteral(", ")) % QStringLiteral("]");
}

QString toString(QDate date)
{
    return date.toString(QStringLiteral("yyyy-MM-dd"));
}

QString toString(QTime time)
{
    return time.toString(QStringLiteral("hh:mm:ss.zzz"));
}

QString toString(const QDateTime &datetime)
{
    return toString(datetime.date()) % QStringLiteral("T") % toString(datetime.time()) + datetime.timeZoneAbbreviation();
}

QString toString(const QString &string)
{
    return QStringLiteral("\"") % escape(string) % QStringLiteral("\"");
}

QString escape(const QString &string)
{
    QString copy(string);
    return copy.replace('"', QLatin1String("\\\""))
           .replace('\n', QLatin1String("\\n"))
           .replace('\r', QLatin1String("\\r"))
           .replace('\t', QLatin1String("\\t"));
}

QString toString(const QVariant &value, int userType)
{
    Q_UNUSED(value)
    return QStringLiteral("{UserType: %1").arg(userType);
}

bool isOpenBracket(const QStringList &brackets, QChar c)
{
    for (const auto &b : brackets)
    {
        if (!b.isEmpty() && b[0] == c)
            return true;
    }
    return false;
}

bool isCloseBracket(const QStringList &brackets, QChar c)
{
    for (const auto &b : brackets)
    {
        if (b.length() > 1 && b[1] == c)
            return true;
    }
    return false;
}

QString Logging::indentString(const QString &string, const QStringList &indentBrackets)
{
    QString result;
    QChar quote{'\0'};
    QChar last{'\0'};
    QChar current{'\0'};
    int indent = 0;

    for (int i = 0; i < string.length(); ++i)
    {
        current = string[i];

        if (quote != QChar('\0'))
        {
            if (quote == current)
                quote = '\0';
        }
        else
        {
            switch (current.toLatin1())
            {
            case '\'':
            case '"':
                quote = current;
                break;
            case ',':
                result += "\n" + QString(indent, QChar(' '));
                break;
            default:
                if (isOpenBracket(indentBrackets, current))
                {
                    result += "\n" + QString(indent, QChar(' '));
                    indent++;
                }
                else if (isCloseBracket(indentBrackets, current))
                {
                    indent--;
                    result += "\n" + QString(indent, QChar(' '));
                }
                break;
            }
        }
        if (isOpenBracket(indentBrackets, last))
        {
            if (!isCloseBracket(indentBrackets, current))
                result += "\n" + QString(indent, QChar(' '));
        }
        result += QString(current);
        last = string[i];
    }
    return result;
}

