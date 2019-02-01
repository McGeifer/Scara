// vector.h

#ifndef _VECTOR_h
#define _VECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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

enum vector
{
	X,
	Y,
	Z,
	V_SIZE
};

// Vektoren Addieren v1+v2=v3 
void vAdd(float *v1[V_SIZE], float *v2[V_SIZE], float *v3[V_SIZE]);

// Vektoren Subtrahieren v1-v2=v3 
void vSub(float v1[V_SIZE], float v2[V_SIZE], float v3[V_SIZE]);

// Vektor multiplizieren mit Zahl v1*z=v2 
void vMultF(float v1[V_SIZE], float z, float v2[V_SIZE]);

// Vektor Betrag (Länge) von v1 oder |v1|=z 
float vBetrag(float v1[V_SIZE]);

// Vektoren Skalar multiplizieren v1 * v2 = z 
// |V1| * |V2| * cos winkel_dawischen = z
// Wenn das Ergebis 0 ist, dann sind die Vektoren senkrecht zueinander
float vSkalarprodukt(float v1[V_SIZE], float v2[V_SIZE]);

// Den Winkel in Grad zwischen zwei Vektoren berechnen  
float vWinkelGrad(float v1[V_SIZE], float v2[V_SIZE]);

// Vektorprodukt v1 x v2 = v3
// v3 steht senkrecht auf v1 und v2
// v3 Betrag |v3| ist die Fläche vom v1 und v2 Parallelogramm
// Winkel zwischen v1 und v2 ist 
// |v1 x v2| = |v3| = |A| * |B| * sin winkel_dawischen
void vVektorprodukt(float v1[V_SIZE], float v2[V_SIZE], float v3[V_SIZE]);

// Einheitsvektor v1 / |v1| = v2
// Ergibt den Vektor mit der Länge 1
void vEinheitsvektor(float v1[V_SIZE], float v2[V_SIZE]);

// Der RichtungskosinusX bildet den Winkel zwischen dem Vektor und der X Achse
// cos(w) = vx / |v|
float vRichtungscosinusX(float v1[V_SIZE]);

// Der RichtungskosinusY bildet den Winkel zwischen dem Vektor und der Y Achse
// cos(w) = vy / |v|	
float vRichtungscosinusY(float v1[V_SIZE]);

// Der RichtungskosinusZ bildet den Winkel zwischen dem Vektor und der Z Achse
// cos(w) = vz / |v|
float vRichtungscosinusZ(float v1[V_SIZE]);
	
#endif

