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
#include <tbb/tbb.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "time.h"
#include <ctime>
#include <tbb/tick_count.h>
using namespace tbb;
using namespace std;
using namespace std::chrono;

void InitializationMatrix(double* MatrixA, double* MatrixB, int Size)
{
	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < Size; j++)
		{
			MatrixA[i*Size + j] = (rand() % 10000) / 1000.0f;
			MatrixB[i*Size + j] = (rand() % 10000) / 1000.0f;
		}
	}
}

int matrix_equal(double *matrA, double *matrB, int Size)
{
	int equal = 1;
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			if (abs((int)(matrA[i*Size + j] - matrB[i*Size + j])) > 0.01)
				equal = 0;
	return equal;
}

bool MultiplicationCheck(double* MatrixA, double* MatrixB, double* MatrixRC, double* MatrixR, int Size)
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

void Mult(double* MatrixA, double* MatrixB, double* MatrixR, int Size, int BSize, int jj, int kk, int ii)
{
	for (int j = 0; j < BSize; j++)
		for (int k = 0; k < BSize; k++)
			for (int i = 0; i < BSize; i++)
				MatrixR[(j + jj) * Size + (ii + i)] += MatrixA[(jj + j) * Size + (kk + k)] * MatrixB[(kk + k)*Size + (ii + i)];
	//		cout << "A:" << MatrixA[(jj + j) * Size + (kk + k)] << endl;
	//		cout << "B:" << MatrixB[(kk + k) * Size + (ii + i)] << endl;
	//		cout << "R:" << MatrixR[(j + jj) * Size + (ii + i)] << endl;
	//	}cout << "-";
	//}cout << "--";
}


void Method(double* MatrixA, double* MatrixB, double* MatrixR, int Size, int BSize)
{
//	omp_set_num_threads(2);
//  #pragma omp parallel for schedule(dynamic)

	tbb::parallel_for(tbb::blocked_range<size_t>(0, Size / BSize), [&](const tbb::blocked_range<size_t>& r) {
		for (size_t j = r.begin(); j != r.end(); ++j)
			for (int k = 0; k < Size; k += BSize)
				for (int i = 0; i < Size; i += BSize)
					Mult(MatrixA, MatrixB, MatrixR, Size, BSize, j*BSize, k, i);
	}, tbb::affinity_partitioner());
}

void PrintMatrix(double *matrix, int rows, int cols)
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
	tbb::task_scheduler_init init(4);

	double *MatrixA = NULL;
	double *MatrixB = NULL;
	double *Result = NULL;
	double *ResultCheck = NULL;
	int Size, BSize;
	high_resolution_clock::time_point t11, t12, t21, t22;

	cout << "Matrix Fox multiplication: " << endl;

	Size = stoi(string(argv[1]));
	BSize = stoi(string(argv[2]));

	if (Size % BSize != 0)
	{
		cout << "Error: wrong Size and Block Size!" << endl;
	}
	else
	{
		MatrixA = new double[Size*Size]{ 0 };
		MatrixB = new double[Size*Size]{ 0 };
		Result = new double[Size*Size]{ 0 };
		ResultCheck = new double[Size*Size]{ 0 };
		InitializationMatrix(MatrixA, MatrixB, Size);

		tick_count start = tick_count::now();
		Method(MatrixA, MatrixB, Result, Size, BSize);
		tick_count finish = tick_count::now();
		init.terminate();

		tbb::task_scheduler_init init(1);
		tick_count start1 = tick_count::now();
		Method(MatrixA, MatrixB, ResultCheck, Size, BSize);
		tick_count finish1 = tick_count::now();
		init.terminate();

		if (Size < 11)
		{
			cout << "Matrix1 " << endl;
			PrintMatrix(MatrixA, Size, Size);
			cout << endl << "Matrix2 " << endl;
			PrintMatrix(MatrixB, Size, Size);
			cout << endl << "TBB mul" << endl;
			PrintMatrix(Result, Size, Size);
			cout << endl << "Basic mul" << endl;
			PrintMatrix(ResultCheck, Size, Size);
			cout << endl;
		}
		cout << "TBB time: " << (finish - start).seconds() << " mcs" << endl;
		cout << "Seq time: " << (finish1 - start1).seconds() << " mcs" << endl;
		cout << "Acceleration = " << (finish1 - start1).seconds() / (finish - start).seconds() << endl;

		if (matrix_equal(Result, ResultCheck, Size)) cout << "Answer is correct\n";
		else cout << "Answer is incorrect\n";

		delete[] MatrixA;
		delete[] MatrixB;
		delete[] Result;
		delete[] ResultCheck;
	}
	system("pause");
	return 0;
}