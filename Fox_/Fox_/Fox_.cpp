// Fox_.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

void InitializationMatrix(int* MatrixA, int* MatrixB, int Size)
{
	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < Size; j++)
		{
			MatrixA[i*Size + j] = rand() % 2;//9
			MatrixB[i*Size + j] = rand() % 2;
		}
	}
}

bool MultiplicationCheck(int* MatrixA, int* MatrixB, int* MatrixRC, int* MatrixR, int Size)
{
	bool flag = false;
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			for (int k = 0; k < Size; k++)
			{
				MatrixRC[i*Size + j] += MatrixA[i*Size + k] * MatrixB[k*Size + j];
			}

	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
		{
			if (MatrixRC[i*Size + j] == MatrixR[i*Size + j])
			{
				flag = true;
			}
			else return false;
		}
	return flag;
}

void Mult(int* MatrixA, int* MatrixB, int* MatrixR, int Size, int BSize, int jj, int kk, int ii)
{
	for (int j = 0; j < BSize; j++)
		for (int k = 0; k < BSize; k++)
			for (int i = 0; i < BSize; i++)
				MatrixR[(j + jj) * Size + (ii + i)] += MatrixA[(jj + j) * Size + (kk + k)] * MatrixB[(kk + k)*Size + (ii + i)];
				//{
				//	cout << endl << "MA:" << MatrixA[(jj + j) * Size + (kk + k)] << endl;
				//	cout << endl << "MB:" << MatrixB[(kk + k) * Size + (ii + i)] << endl;
				//	cout << endl << "MR:" << MatrixR[(j + jj) * Size + (ii + i)] << endl;
				//}
}

// метод Фокса - перемножение блоков А и В
void Method(int* MatrixA, int* MatrixB, int* MatrixR, int Size, int BSize)
{
	for (int j = 0; j < Size; j += BSize)
		for (int k = 0; k < Size; k += BSize)
			for (int i = 0; i < Size; i += BSize)
				Mult(MatrixA, MatrixB, MatrixR, Size, BSize, j, k, i);
}

void PrintMatrix(int *matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			cout << matrix[i * cols + j] << " ";
		}
		cout << endl;
	}
}

int main(int argc, char *argv[])
{
	int *MatrixA = NULL;
	int *MatrixB = NULL;
	int *Result = NULL;
	int *ResultCheck = NULL; 
	int Size, BSize; //размер матричного блока
	high_resolution_clock::time_point t1, t2;

	cout << "Matrix Fox multiplication: " << endl;

	Size = stoi(string(argv[1]));
	BSize = stoi(string(argv[2]));

	if (Size % BSize != 0)
	{
		cout << "Error: wrong Size and Block Size!" << endl;
	}
	else
	{
		MatrixA = new int[Size*Size]{ 0 };
		MatrixB = new int[Size*Size]{ 0 };
		Result = new int[Size*Size]{ 0 };
		ResultCheck = new int[Size*Size]{ 0 };
		InitializationMatrix(MatrixA, MatrixB, Size);
		if (Size < 11)
		{
			cout << "Matrix A" << endl;
			PrintMatrix(MatrixA, Size, Size);
			cout << endl << "Matrix B" << endl;
			PrintMatrix(MatrixB, Size, Size);
			cout << endl;
		}

		t1 = high_resolution_clock::now();

		Method(MatrixA, MatrixB, Result, Size, BSize);

		t2 = high_resolution_clock::now();

		bool CheckFlag = MultiplicationCheck(MatrixA, MatrixB, ResultCheck, Result, Size);//последовательное умножение матриц

		if (Size < 11)
		{
			cout << endl << "Matrix C" << endl;
			PrintMatrix(Result, Size, Size);
			cout << endl;
		}
		cout << "Time of method is: " << duration_cast<microseconds>(t2 - t1).count() << " mcs" << endl;
		if(CheckFlag == 1)
			cout << "Checking of method is TRUE (correct)" << endl;
		else
			cout << "Checking of method is FALSE (uncorrect)" << endl;

		delete[] MatrixA;
		delete[] MatrixB;
		delete[] Result;
		delete[] ResultCheck;
	}
	return 0;
}