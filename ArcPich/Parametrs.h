#pragma once
#include<cmath>
float dy = -5; //(rand() % 10 + 5);//��������� ������ ������ ������
float dx = 1;//-(1 - dy);//��������� ������ ������ ������
int /*dx=0, dy=-5,*/ dy1 = dy; //dy1 = �������� ����� ������
int TimPer = 16;
int ballspeed = 0;
const int steps = sqrt(dy * dy + dx * dx); // ����� ��������, ��������� ������ ���������� ��������� ����� ��� � ��� ������ �������� ������ �� 1 ���� 
//�������� ���������� ��� �������� ��������
double stepDx = dx / steps;
double stepDy = dy / steps;
int newX;
int newY;
int side;
int ballsize = 20;