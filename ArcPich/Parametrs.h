#pragma once
#include<cmath>
float dy = -45; //(rand() % 10 + 5);//��������� ������ ������ ������
float dx = 2;//-(1 - dy);//��������� ������ ������ ������
int /*dx=0, dy=-5,*/ dy1 = -3; //dy1 = �������� ����� ������
int TimPer = 500;
int ballspeed = 0;
const int steps = sqrt(dy * dy + dx * dx); // ����� ��������, ��������� ������ ���������� ��������� ����� ��� � ��� ������ �������� ������ �� 1 ���� 
//�������� ���������� ��� �������� ��������
double stepDx = dx / steps;
double stepDy = dy / steps;
int newX;
int newY;
int side = 0;
int ballsize = 20;