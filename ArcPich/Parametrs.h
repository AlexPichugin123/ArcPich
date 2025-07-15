#pragma once
#include<cmath>
float dy = -45; //(rand() % 10 + 5);//формируем вектор полета шарика
float dx = 2;//-(1 - dy);//формируем вектор полета шарика
int /*dx=0, dy=-5,*/ dy1 = -3; //dy1 = скорость после сброса
int TimPer = 500;
int ballspeed = 0;
const int steps = sqrt(dy * dy + dx * dx); // число подшагов, поставить равным количеству пикслелей между нач и кон точкой движения шарика за 1 кадр 
//алгоритм брезенхема для проверки коллизии
double stepDx = dx / steps;
double stepDy = dy / steps;
int newX;
int newY;
int side = 0;
int ballsize = 20;