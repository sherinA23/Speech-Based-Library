// MyLibraryApp.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace MyLibraryApp;

long double A[6][6] = {0}, B[6][33] = {0}, Pi[6] = {0};
int O[21][86] = {0};
long double Del[86][6] = {0};
int Psi[86][6] = {0}, Q[86] = {0};

long double alpha[86][6] = {0}, beta[86][6] = {0};
long double Xi[86][6][6] = {0}, gamma[86][6] = {0};
long double A_opt[10][6][6] = {0}, B_opt[10][6][86] = {0}, Pi_opt[10][6] = {0};


float arr[45000];  // array which stores the values of all samples in the file

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}
