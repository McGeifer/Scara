// Funktionen für die Vektor Rechnung 
//
// Matthias Busse 1.10.2018 Version 1.0
// Inhalt:
// vAdd(v1, v2, v3)           : v1+v2=v3
// vSub(v1, v2, v3)           : v1-v2=v3
// vMultF(v1, f, v2)          : v1*f=v2
// f=vBetrag(v1)              : |v1|=f
// f=vSkalarprodukt(v1, v2)   : v1 * v2=f
// f=vWinkelGrad(v1, v2)      : Winkel zwischen v1 und v2 in Grad
// vVektorprodukt(v1, v2, v3) : v1 x v2 = v3
// vEinheitsvektor(v1, v2)    : v1 / |v1| = v2, Länge=1
// vRichtungscosinusX(v1)     : v1 Winkel zur X-Achse in Grad
// vRichtungscosinusY(v1)     : v1 Winkel zur Y-Achse in Grad
// vRichtungscosinusZ(v1)     : v1 Winkel zur Z-Achse in Grad
//      z |
//        |
//        |
//        ---------- y
//       /
//    x /

#include "vector.h"

void vAdd(float *v1[V_SIZE], float *v2[V_SIZE], float *v3[V_SIZE])
{
	*v3[X] = (float)*v1[X] + *v2[X];
	*v3[Y] = (float)*v1[Y] + *v2[Y];
	*v3[Z] = (float)*v1[Z] + *v2[Z];
}
void vSub(float v1[V_SIZE], float v2[V_SIZE], float v3[V_SIZE])
{
	v3[X] = (float)v1[X] - v2[X];
	v3[Y] = (float)v1[Y] - v2[Y];
	v3[Z] = (float)v1[Z] - v2[Z];
}
void vMultF(float v1[V_SIZE], float z, float v2[V_SIZE])
{
	v2[X] = (float)v1[X] * z;
	v2[Y] = (float)v1[Y] * z;
	v2[Z] = (float)v1[Z] * z;
}
float vBetrag(float v1[V_SIZE])
{
	float z;
	z = sqrt((float)v1[X] * v1[X] + v1[Y] * v1[Y] + v1[Z] * v1[Z]);
	return z;
}
float vSkalarprodukt(float v1[V_SIZE], float v2[V_SIZE])
{
	float z;
	z = ((float)v1[X] * v2[X]) + (v1[Y] * v2[Y]) + (v1[Z] * v2[Z]);
	return z;
}
float vWinkelGrad(float v1[V_SIZE], float v2[V_SIZE])
{
	float z, w;
	z = vSkalarprodukt(v1, v2);
	w = (float)z / vBetrag(v1) / vBetrag(v2);
	return acos(w) * RAD_TO_DEG;
}
void vVektorprodukt(float v1[V_SIZE], float v2[V_SIZE], float v3[V_SIZE])
{
	v3[X] = (float)(v1[Y] * v2[Z]) - (v1[Z] * v2[Y]);
	v3[Z] = (float)(v1[X] * v2[Y]) - (v1[Y] * v2[X]);
	v3[Y] = -1.0 * ((v1[X] * v2[Z]) - (v1[Z] * v2[X]));
}
void vEinheitsvektor(float v1[V_SIZE], float v2[V_SIZE])
{
	float betrag;
	betrag = vBetrag(v1);
	v2[X] = (float)v1[X] / betrag;
	v2[Y] = (float)v1[Y] / betrag;
	v2[Z] = (float)v1[Z] / betrag;
}
float vRichtungscosinusX(float v1[V_SIZE])
{
	float winkel;
	winkel = acos((float)v1[X] / vBetrag(v1));
	return winkel * RAD_TO_DEG;
}
float vRichtungscosinusY(float v1[V_SIZE])
{
	float winkel;
	winkel = acos((float)v1[Y] / vBetrag(v1));
	return winkel * RAD_TO_DEG;
}
float vRichtungscosinusZ(float v1[V_SIZE])
{
	float winkel;
	winkel = acos((float)v1[Z] / vBetrag(v1));
	return winkel * RAD_TO_DEG;
}
