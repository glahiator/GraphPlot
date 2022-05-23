#include "CalculateCylinder.h"

CalculateCylinder::CalculateCylinder()
{
    vals = CylinderVals();
    isInited = false;
}

void CalculateCylinder::InitValues(CylinderVals _val)
{
    vals = _val;
    isInited = true;
}

double CalculateCylinder::GetStockLosses()
{
    if( !isInited ) return 0.0;
    double Qthr = vals.plc.VS * Ar1 * 6;
    double Qrealr = vals.sens.fT_L / 2.07;
    double phi_r = Qrealr / Qthr;
    return phi_r * 100;
}

double CalculateCylinder::GetPistonLosses()
{
    if( !isInited ) return 0.0;
    double Qthp = vals.plc.VS * Ap1 * 6;
    double Qrealp = vals.sens.fT_L * 2.07;
    double phi_p = Qrealp / Qthp ;
    return phi_p * 100;
}

double CalculateCylinder::GetForce()
{
    if( !isInited ) return 0.0;
    double Fthp = pS * Ap1 / 100.0;
    double Fthr = pS * Ar1 / 100.0;
    double Fth = Fthp - Fthr;
    double Freal = ( vals.plc.pA * Ap1 * 2.07 - vals.plc.pB * Ar1) / 59.07;
    double ntetta = Fth / Freal;
    return ntetta;
}
