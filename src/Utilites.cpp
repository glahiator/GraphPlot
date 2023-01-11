#include "Utilites.h"

qint64 GetCurrentTime1()
{
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

// UTILS FOR SENDING AND PARSING PACKS
quint8  HighByte(quint16 value) { return quint8(value >> 8);}
quint8  LowByte(quint16 value)  { return quint8(value & quint8(0xff)); }
quint8  Get32_hh(qint32 value)   { return quint8(value>>24); }
quint8  Get32_h(qint32 value)    { return quint8(value>>16); }
quint8  Get32_L(qint32 value)    { return quint8(value>>8); }
quint8  Get32_Ll(qint32 value)   { return quint8(value); }
quint8  GetU_32hh(quint32 value) { return quint8(value>>24); }
quint8  GetU_32h(quint32 value)  { return quint8(value>>16); }
quint8  GetU_32l(quint32 value)  { return quint8(value>>8);  }
quint8  GetU_32ll(quint32 value) { return quint8(value); }
quint16 Uniq(quint8 highByte, quint8 lowByte) { return ((quint16(highByte) << 8)|lowByte); }
qint32  Make32Value(quint8 hh, quint8 h, quint8 l, quint8 ll)    { return qint32((qint32(hh)<<24) | (qint32(h)<<16) | (qint32(l)<<8) | qint32(ll)); }
quint32 MakeU_32Value(quint8 hh, quint8 h, quint8 l, quint8 ll)  { return quint32((quint32(hh)<<24) | (quint32(h)<<16) | (quint32(l)<<8) | quint32(ll)); }
quint16 calcCRC(quint8 *data, quint8 size)
{
    quint16 crc = 0xFFFF;
    while (size--){
        crc ^= *data++ << 8;
        for (quint8 i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
   return crc;
}

quint16 calcCRC16(quint8 *data, quint16 size)
{
    quint16 crc = 0xFFFF;
    while (size--){
        crc ^= *data++ << 8;
        for (quint8 i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }
   return crc;
}

// UTILS FOR MAPPING AND CONSTRAINING
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
double constrain(double value,double low,double high)
{
    return ((value)<(low)?(low):((value)>(high)?(high):(value)));
}
