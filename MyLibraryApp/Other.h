#pragma once
#include <iostream>
using namespace std;
ref class Other
{
	long double **A, **B, *Pi, **Del, **alpha, **beta, **gamma, ***Xi;
	int **O, **Psi, *Q;
	long double ***A_opt, ***B_opt, **Pi_opt;
	float *arr;  // array which stores the values of all samples in the file

public:
	Other(void);
	int add();
	
	void modify_data(int num_of_samples, int max_norm_val)  // function to normalise and perform dc_shift
	{
		float sum = 0, max = 0, DC_shift = 0;
		int i = 0;

		//Calculating DC Shift

		for(i = 0 ; i < num_of_samples ; i++) 
		{
			sum = sum + arr[i];

			if(abs(arr[i]) > max)
				max = arr[i];        //finding max value which will be used for normalisation
		}

		DC_shift = sum / num_of_samples ;


		//Normalisation

		for(i = 0 ; i < num_of_samples ; i++)
		{
			arr[i] = (arr[i] - DC_shift) * (max_norm_val / max);  // here we want max_norm_val to be +5000
		}

	}
	int find_steadyFrames(int num_of_samples)  // function to find the frame with highest STE
	{
		int i = 0, j = 0, index = 0;
		float energy = 0, max = 0;

		for(i = 0; i < num_of_samples; i = i + 320)
		{

			energy = 0;

			for(j = i; j < i+320 && j < num_of_samples; j++)    
			{
				energy = energy + arr[j]*arr[j];    // compute the energy of every frame
			}

			if(max < energy)  
			{
				max = energy;
				index = i;
			}

		}

		return index;
	}
	void find_Ci(long double C[], int start)    // function to find the C[i] values or Cepstral coefficients of a given frame
	{
		float R[13] = {0}, Alpha[13][13] = {0}, E[13] = {0}, K[13] = {0}, sum = 0;
		int i = 0, j = 0, S = 320, p = 12, m = 0, k = 0;

		for(i = 0; i <= p; i++)
		{
			for(j = start; j <= start+S-i-1; j++) 
			{
				R[i] = R[i] + arr[j]*arr[j+i];   // computing the R[i] values first
			}
		}


		// levinson-durbin algo to find A[i] values

		E[0] = R[0];     //initialization

		for(i = 1; i <= p; i++) 
		{
			sum = 0;

			for(j = 1; j <= i-1; j++) 
			{
				sum = sum + Alpha[i-1][j]*R[i-j];
			}

			K[i] = (R[i] - sum)/E[i-1];

			Alpha[i][i] = K[i];

			for(j = 1; j <= i-1; j++) 
			{
				Alpha[i][j] = Alpha[i-1][j] - K[i]*Alpha[i-1][i-j];
			}

			E[i] = (1 - K[i]*K[i])*E[i-1];	

		}

		// now compute the C[i] values

		C[0] = log(R[0]*R[0]);

		for(m = 1; m <= p; m++)
		{
			sum = 0;

			for(k = 1; k <= m-1; k++) 
			{
				sum = sum + (1.0*k/m)*C[k]*Alpha[12][m-k];
			}

			C[m] = Alpha[12][m] + sum;
		
		}



		//performing raised sine window on C[i] values

		for(m = 1; m <= p; m++)
		{
			C[m] = C[m]*(1+ (p/2)*sin(3.14*m/p));
		}
	

	}
	void generate_Ci_utterance(char* c1, long double C[][85][13], int m)  // function to compute C[i] values of all frames given a recording/utterance
	{
		FILE *fp;
		int i = 0, j = 0, k = 0, total = 0, start = 0, count = 0;
		char str[20], dummy[30];

		fp = fopen(c1,"r");

//		if(fp == NULL)
//			printf("error: file cannot be opened");

		count = 0;

		while(count < 5)  //skipping the first 5 lines which contains the header data
		{
			fgets(dummy, 30, fp);
			count++;
		}

		k = 0;

		while(fscanf(fp, "%f", &arr[k]) != EOF)  // copying the values from the text file into an array
		{
			if(k == 44000)
				break;
			k++;
		}

		total = k;   // 'total' denotes total number of samples in that file

		modify_data(total,5000);   // normalising the data


		start = find_steadyFrames(total);  // start is the middle frame

		int index = (total - start) / 320;

		int ind = 85 - index - 10;

		ind > 0 ? 1:(ind = 0);

		for(i = 1; i < ind*320; i++)
		{
			float num = (rand() % 10) + 1;  // if we don't reach 85 frames by the end of the signal, add silence
			arr[i+total] = num/5000;
		}

		start = start - 3200;   // take 10 frames to it's left

		j = 0;
	
		while(j < 85)  // finding the C[i] values of all the 85 frames
		{
			find_Ci(C[m][j], start);
			start = start + 320;
			j++;
		}


		fclose(fp);

	}
	void generate_Ci_digit(int numof_files, long double C[][85][13], char* roll, char* v, int start)  // function to generate cepstral coefficients for every utterance of a digit
	{
		int i = 0, j = 0, m = 0;
		char str[5];

		for(i = 1+start; i <= start+numof_files; i++)   // this loops over many utterance files of a digit
		{
			char c1[40] = "";
			strcpy(c1,roll);
			strcat(c1,"_");

			strcat(c1,v);
			strcat(c1,"_");
			sprintf(str, "%d", i);
			strcat(c1,str);
			strcat(c1,".txt");  // obtaining the file name

			generate_Ci_utterance(c1, C, m);  // finding C[i] values
			m++;

		}

	}
	void loadUniverse(long double* X[], char* filename, int numof_obs, int num_words)   // read the universe vectors from the txt file and load them into an array
	{
		FILE *fp;
		int rows = 85*numof_obs*(num_words+1);
		int i = 0, j = 0, k = 0;
		char str[150], ch;

		fp = fopen(filename,"r");

//		if(fp == NULL)
//			printf("error opening file");

	
		for(i = 0; i < rows; i++) 
		{
			j = 0;

			fgets(str, 150, fp);
			char* token = strtok(str, " ");   // the values are separated by space, so strtok helps in tokenising
  
			while (token != NULL && j < 12) 
			{
				sscanf(token,"%Lf",&X[i][j]);
				j++;
				token = strtok(NULL, " ");   

			}

		}

		fclose(fp);

	}
	long double TokhuraDist(long double x[], long double y[])  // calculating Tokhura's distance between two vectors
	{
		int i = 0;
		long double sum = 0;
		long double w[12] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};   // Tokhura weights

		for(i = 0; i < 12; i++)
		{
			sum = sum + w[i] * ( pow(x[i] - y[i], 2) );  // Tokhura's distance
		}

		return sum;
	}
	long double forward(int obs_no)    // function to implement forward procedure
	{
		int i = 0, j = 0, t = 0;
		int T = 85, N = 5;
		long double sum = 0, prob = 0;

		for(i = 1; i <= N; i++)      // initialization
		{
			alpha[1][i] = Pi[i]*B[i][O[obs_no][1]];
		}


		for(t = 1; t <= T-1; t++)      // induction step of the algorithm
		{
			for(j = 1; j <= N; j++)
			{
				sum = 0;

				for(i = 1; i <= N; i++)
					sum = sum + (alpha[t][i] * A[i][j]);

				alpha[t+1][j] = sum * B[j][O[obs_no][t+1]];
			}
		}


		for(i = 1; i <= N; i++)        // termination
			prob = prob + alpha[T][i];

		return prob;

	}
	void load_model(long double y[][12], int num_words,int p)
	{
		int i = 0, j = 0, k = 0, seq = 0;
		int N = 5, M = 32;
		char ch;
		FILE* fg;

		char str[5];
		char c1[20] = "model";
		sprintf(str, "%d", p);
		strcat(c1,str);
		strcat(c1,".txt");


		fg = fopen(c1,"r");

		for(seq = 0; seq <= num_words; seq++)                // we check the probability with every digit in the trained model
		{
			for(i = 1; i <= N; i++)                  // load the converged models into A,B and Pi
			{
				fscanf(fg,"%Lf%c",&Pi_opt[seq][i], &ch) ;
			}

			for(i = 1; i <= N; i++)                  // load the converged models into A,B and Pi
			{
				for(j = 1; j <= N; j++)
				{
					fscanf(fg,"%Lf%c",&A_opt[seq][i][j],&ch); 
				}
			}

			for(i = 1; i <= N; i++)                  // load the converged models into A,B and Pi
			{
				for(k = 1; k <= M; k++)
				{
					fscanf(fg,"%Lf%c",&B_opt[seq][i][k],&ch); 
				}
			}

		}
		fclose(fg);

		char c2[20] = "codebook";
		sprintf(str, "%d", p);
		strcat(c2,str);
		strcat(c2,".txt");


		fg = fopen(c2,"r");

		for(i = 0; i < 32; i++)
		{
			for(j = 0; j < 12;j++)
			{
				fscanf(fg,"%Lf%c",&y[i][j], &ch);
	
			}
		}

		fclose(fg);
	}

	int live_testing(int num_words)   // function to perform live testing
	{
		long double y[32][12] = {0};
		int i = 0, num = 0, choice = 0, p = 0;
		char roll2[30] = "unique/214101046";
		long double* X[7000];

		for (i = 0; i < 7000; i++)
			X[i] = (long double*)malloc(12 * sizeof(long double));

		int j = 0, k = 0, N = 5, M = 32;
		long double Coeff[1][85][13] = {0};
		FILE *fp;
		int total = 0, start = 0, count = 0, digit = 0, prediction = 0, index = 0; 
		long double prob = 0, max = 0, min = 0;
		char* testfile = "livetest_coeff.txt";

		system("Recording_Module.exe 3 live.wav live.txt");   // system command to do live recording
	
		generate_Ci_utterance("live.txt", Coeff, 0);   // generate the cepstral coefficients

		fp = fopen(testfile, "w");

		for(int k = 0; k < 85; k++)
		{
			for(int n = 1; n <= 12; n++)
			{
				fprintf(fp,"%Lf ",Coeff[0][k][n]);
			}

			fprintf(fp,"\n");
		}

		fclose(fp);

		if(num_words == 9)
			p = 1;
		else
			p = 2;

		load_model(y,num_words,p);

		loadUniverse(X,testfile,1,num_words);  // load it into X

		for(i = 0; i < 85; i++)
		{
			min = INT_MAX;
			for(j = 0; j < 32; j++)
			{
				if(TokhuraDist(X[i],y[j]) < min)
				{
					min = TokhuraDist(X[i],y[j]);
					index = j;
				}	
			}
			O[1][i+1] = index+1;  // computing the obs sequence
		}



		prediction = 0;
		max = 0;

		for(digit = 0; digit <= num_words; digit++)  // we check the probability with every digit in the trained model
		{
			for(i = 1; i <= N; i++)   // load the converged models into A,B and Pi
			{
				Pi[i] = Pi_opt[digit][i];

				for(j = 1; j <= N; j++)
				{
					A[i][j] = A_opt[digit][i][j];
				}
				for(k = 1; k <= M; k++)
				{
					B[i][k] = B_opt[digit][i][k];
				}
			}

			prob = forward(1);

			if(prob > max)   // the one with the maximum probability wins
			{
				max = prob;
				prediction = digit;
			}

		}
		return prediction;
	}
};

