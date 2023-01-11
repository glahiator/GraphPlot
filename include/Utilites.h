#ifndef UTILITES_H
#define UTILITES_H
#include <QtMath>
#include <chrono>

qint64 GetCurrentTime1();
// UTILS FOR SENDING AND PARSING PACKS
quint16 calcCRC(quint8 *data, quint8 size);
quint16 calcCRC16(quint8 *data, quint16 size);
quint8  LowByte(quint16 value);
quint8  HighByte(quint16 value);
quint16 Uniq(quint8 HighByte, quint8 LowByte);
qint32  Make32Value(quint8 hh, quint8 h, quint8 l, quint8 ll);
quint8  Get32_hh(qint32 value);
quint8  Get32_h(qint32 value);
quint8  Get32_l(qint32 value);
quint8  Get32_ll(qint32 value);
quint32 MakeU_32Value(quint8 hh, quint8 h, quint8 l, quint8 ll);
quint8  GetU_32hh(quint32 value);
quint8  GetU_32h(quint32 value);
quint8  GetU_32l(quint32 value);
quint8  GetU_32ll(quint32 value);

// UTILS FOR MAPPING AND CONSTRAINING
double map(double x, double in_min, double in_max, double out_min, double out_max);
double constrain(double value,double low,double high);

#endif // UTILITES_H
