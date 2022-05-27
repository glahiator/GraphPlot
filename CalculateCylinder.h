#ifndef CALCULATECYLINDER_H
#define CALCULATECYLINDER_H

#include "PLC_Connector.h"
#include "Sensor.h"

const double Dp1 = 125.0; //Диаметр поршня первого левого цилиндра
const double Dst1 = 90.0; // Диаметр штока левого цилиндра
const double S1 = 25.0; // Ход левого цилиндра
const double Ap1 = 122.65; // Площадь поршневой полости левого цилиндра
const double Ar1 = 59.07; // Площадь штоковой полости левого цилиндра
const double pS = 250.0; // Системное давление
const double Dp2 = 125.0; //Диаметр поршня первого правого цилиндра
const double Dst2 = 90.0; // Диаметр штока правого цилиндра
const double S2 = 25.0; // Ход правого цилиндра
const double Ap2 = 122.65; // Площадь поршневой полости правого цилиндра
const double Ar2 = 59.07; // Площадь штоковой полости правого цилиндра


struct cylinder_vals1 {
    cylinder_vals1() {
        sens = SensorVals();
        plc = plc_cylinder();
    }
    SensorVals sens;
    plc_cylinder plc;
};

class CalculateCylinder
{
public:
    CalculateCylinder();

    void InitValues( cylinder_vals1 _val );
    double GetStockLosses();
    double GetPistonLosses();
    double GetForce();

private:
    cylinder_vals1 vals;
    bool isInited;
};

#endif // CALCULATECYLINDER_H
