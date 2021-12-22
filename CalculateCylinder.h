#ifndef CALCULATECYLINDER_H
#define CALCULATECYLINDER_H

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


struct CylinderVals {
    CylinderVals() {
        pA = 0.0;
        pB = 0.0;
        sY = 0.0;
        fY = 0.0;
        fS = 0.0;
        fT = 0.0;
        VS = 0.0;
        tfT = 0.0;
    }
    double pA; // Давление в поршневой полости  цилиндра
    double pB; // Давление в штоковой полости  цилиндра
    double sY; // Задание на проп. клапан  цилиндра
    double fY; // Обратная связь положения золотника проп. клапана  цилиндра
    double fS; // Обратная связь положения штока  цилиндра
    double fT; // Объемный расход из блока  цилиндра
    double VS; // Обратная связь скорости штока  цилиндра
    double tfT; // Температура расхода из блока  цилиндра
};

class CalculateCylinder
{
public:
    CalculateCylinder();

    void InitValues( CylinderVals _val );

    double GetStockLosses();
    double GetPistonLosses();
    double GetForce();

    bool isInited;
    CylinderVals vals;

};

#endif // CALCULATECYLINDER_H
