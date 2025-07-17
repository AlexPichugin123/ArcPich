#pragma once
#include<cmath>
float dy = -5; //(rand() % 10 + 5);//формируем вектор полета шарика
float dx = 1;//-(1 - dy);//формируем вектор полета шарика
int /*dx=0, dy=-5,*/ dy1 = dy; //dy1 = скорость после сброса
int TimPer = 16;
int ballspeed = 0;
const int steps = sqrt(dy * dy + dx * dx); // число подшагов, поставить равным количеству пикслелей между нач и кон точкой движения шарика за 1 кадр 
//алгоритм брезенхема для проверки коллизии
double stepDx = dx / steps;
double stepDy = dy / steps;
int newX;
int newY;
int side;
int ballsize = 20;