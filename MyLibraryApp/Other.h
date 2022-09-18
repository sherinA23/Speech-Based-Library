#pragma once
#include <iostream>
using namespace std;
ref class Other
{
	long double **A, **B, *Pi, **Del, **alpha, **beta, **gamma, ***Xi, ***A_avg, ***B_avg;
	int **O, **Psi, *Q, trained_words;
	long double ***A_opt, ***B_opt, **Pi_opt, **Pi_avg, **A_bar, **B_bar, *Pi_bar;
	float *arr;  // array which stores the values of all samples in the file

public:
	Other(void);
	
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
	void initCodebook(long double* X[], long double y[][12], int num_words)  // initalising codebook with the centorid of the universe vectors
	{
		long double sum[1][12] = {0}, centroid[1][12] = {0};
		int i = 0, j = 0, M = 0;

		M = 85*20*(num_words+1);

		for(i = 0; i < M; i++)
		{
			for(j = 0; j < 12; j++)
			{
				sum[0][j] = sum[0][j] + X[i][j];   // calculating the sum
			}

		}

		for(j = 0; j < 12; j++)
		{
			centroid[0][j] = sum[0][j] / (M * 1.0);  // dividing the sum values with M to get the centroid value
			y[0][j] = centroid[0][j];
		}

	
	}
	long double K_means(long double y[][12], long double* X[], int k, int num_words)    // applying K-means, given a codebook and Universe vecotrs and k is the size of the codebook
	{
		long double Centroid[32][12] = {0}, count[32] = {0};
		long double D = 0, D_prev = 9999, delta = 0.00001, min = 9999999, dist = 0;
		int i = 0, j = 0, n = 0, index = 0, m = 0;
		int M = 0;

		M = 85*20*(num_words+1);

		while(m < M)
		{
			D = 0;

			for(i = 0; i < 32; i++)
				count[i] = 0;

			for(i = 0; i < M; i++)
			{
				min = 9999999;
				index = 0;

				for(j = 0; j < k; j++)
				{
					dist = TokhuraDist(X[i],y[j]);  // find the distance between X[i] and every vetor in the codebook (i.e., y[j])

					if(dist < min) 
					{
						min = dist;
						index = j;      // index stores the information about which cluster a vector belongs to
					}
				
				}

				D = D + TokhuraDist(X[i],y[index]);   // distortion

				for(n = 0; n < 12; n++)
				{
					Centroid[index][n] = Centroid[index][n] + X[i][n];   // calculating the sum
				}
				

				count[index] = count[index] + 1;     // count stores the no.of vectors in a cluster/region
			
			}


			for(i = 0; i < k; i++) 
			{
				for(n = 0; n < 12; n++)
				{
					Centroid[i][n] = Centroid[i][n]/count[i];   // dividing the sum values with no.of vectors in that respective cluster to get the centroid value
					y[i][n] = Centroid[i][n];
				}
			}

			D = D / (M * 1.0);    // average of distortion values

			//printf("Distortion after iteration %d is %Lf\n",m,D);
			if( abs(D - D_prev) < delta)  // termination condition
				break;

			D_prev = D;

			m++;

		}
	
		return D;   
	}
	void LBG(long double* X[], long double y[][12], char* str, int numof_files, int num_words)   // applying LBG algorithm to generate a codebook
{
	long double e = 0.03, temp = 0, D = 0;
	int i = 0, j = 0, n = 0, m = 0, size = 32;
	FILE* fp;

	fp = fopen("codebook1.txt","w");

	loadUniverse(X,str,numof_files,num_words);   // load the universe vectors into an array

	initCodebook(X,y,num_words);   // intialise the codebook

	m = 1;   // codebook is of size 1 initially

	while(m < size) 
	{
		n = m;
		m = m * 2;   // double the codebook size

		for(i = 0; i < m/2; i++)
		{
			for(j = 0; j < 12; j++)
			{
				temp = y[i][j];
				y[i][j] = temp * (1 + e);   // splitting the codebook
				y[n][j] = temp * (1 - e);
			}

			n = n + 1;
		}

		D = K_means(y,X,m,num_words);   // apply k-means

		//printf("\nDistortion when codebook is of size %d = %Lf\n",m,D);
	}
	
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < 12;j++)
		{
			fprintf(fp,"%Lf ",y[i][j]);
	
		}
		fprintf(fp,"\n");
	}

	fclose(fp);
}
	void write_obs(long double* X[], long double y[][12], int numof_files, int num_words)  // this function writes the obs sequences of each digit into a different file
	{
		int i = 0, j = 0, k = 0, index = 0;
		long double min = 0;
		char str[40] = "Observations/obs_seq_0.txt";
		char s[5];
		FILE* fp;

		fp = fopen(str,"w");

		for(i = 0; i < 85*(num_words+1)*numof_files; i++)
		{
			min = INT_MAX;

			if(i%(85*numof_files) == 0 && i != 0)
			{
				strcpy(str,"Observations/obs_seq_");
				fclose(fp);
				int k = i/(85*numof_files);
				sprintf(s,"%d",k);
				strcat(str,s);
				strcat(str,".txt");
				fp = fopen(str,"w");    // creates the filename of the form "obs_seq_1.txt"
			}


			for(j = 0; j < 32; j++)
			{
				if(TokhuraDist(X[i],y[j]) < min)
				{
					min = TokhuraDist(X[i],y[j]);   // calculates tokhura distance from each vector in the codebook
					index = j;   // store the index of that vector whose distance is min
				}
			}

			fprintf(fp,"%d ",index+1);   // adjusting the index value because codebook indices start from zero but the indices of all the other matrices in HMM process start from 1

			if((i+1)%85 == 0)
			{
				fprintf(fp,"\n");
			}
		}

		fclose(fp);
	}
	void load_obs_seq(int digit, int numof_files)   // load the observation sequences of a digit from the obs seq file into O[][]
	{
		char str[40] = "Observations/obs_seq_";
		char s[5];
		char ch;
		FILE* fp;

		sprintf(s,"%d",digit);
		strcat(str,s);
		strcat(str,".txt");   // obtain the filename
	
	
		fp = fopen(str,"r");

		for(int i = 1; i <= numof_files; i++)
		{
			for(int j = 1; j <= 85; j++)
			{
				fscanf(fp,"%d%c", &O[i][j],&ch);   // loading values into O
			}
		}

		fclose(fp);
	}
	long double viterbi(int obs_no)    // function to perform viterbi algorithm
	{

		int i = 1, j = 1, t = 1, max_index = 0;
		long double max = 0, P_star = 0;
		int N = 5, T = 85;

		for(i = 1; i <= N; i++)      // initialization
		{
			Del[1][i] = Pi[i]*B[i][O[obs_no][1]];
			Psi[1][i] = 0;
		}



		for(t = 2; t <= T; t++)     // recursion step of the algorithm
		{	
			for(j = 1; j <= N; j++)
			{
				max = 0;
				max_index = 0;

				for(i = 1; i <= N; i++)
				{
					if(max < Del[t-1][i]*A[i][j])
					{
						max = Del[t-1][i]*A[i][j];
						max_index = i;
					}
				}

				Del[t][j] = max * B[j][O[obs_no][t]];
				Psi[t][j] = max_index;
			
			}
		}


		max = INT_MIN;

		for(i = 1; i <= N; i++)   // termination
		{
			if(max < Del[T][i])
			{
				max = Del[T][i];
				max_index = i;
			}
		}

		P_star = max;
		Q[T] = max_index;
	

	

		for(t = T-1; t >= 1; t--)    // state sequence back-tracking
		{
			Q[t] = Psi[t+1][Q[t+1]];
		}

		return P_star;
	}
	void backward(int obs_no)     // function to implement backward procedure
	{
		int i = 0, j = 0, t = 0, N = 5, T = 85;
		long double sum = 0;

		for(i = 1; i <= N; i++)   // initialization
			beta[T][i] = 1;

		for(t = T-1; t >= 1; t--)   // induction step of the algorithm
		{
			for(i = 1; i <= N; i++)
			{
				sum = 0;

				for(j = 1; j <= N; j++)
					sum = sum + (A[i][j] * B[j][O[obs_no][t+1]])* beta[t+1][j];
			
				beta[t][i] = sum ;
			}
		}

	}
	void BaumWelch(int obs_no)    // function to perform Baum-Welch procedure
	{
		long double sum = 0, denominator = 0;
		int i = 0, j = 0, t= 0;
		int T = 85, N = 5;

	
		for(t = 1; t <= T-1; t++)  
		{
			denominator = 0;
			for(i = 1; i <= N; i++)
			{
				for(j = 1; j <= N; j++)
					denominator += alpha[t][i] * A[i][j] * B[j][O[obs_no][t+1]] * beta[t+1][j];  // finding the denominator
			}
		
			for(i = 1; i <= N; i++)
				for(j = 1; j <= N; j++)
				{
					Xi[t][i][j] = (alpha[t][i] * A[i][j] * B[j][O[obs_no][t+1]] * beta[t+1][j]) / denominator;   // updating Xi
				}
		}

		for(t = 1; t <= T-1; t++)     
		{
			for(i = 1; i <= N; i++)
			{
				sum = 0;
				for(j = 1; j <= N; j++)    
					sum = sum + Xi[t][i][j];      // summing the Xi values to get gamma

				gamma[t][i] = sum;
			}
		
		}

	}
	void re_estimation(int obs_no)    // function to do re-estimation
	{
		int i = 0, j = 0, t = 0, k = 0, ith = 0, jth = 0;
		long double sum1 = 0, sum2 = 0, diff = 0, maximum = 0;
		int T = 85, N = 5, M = 32;
	
		for(i = 1; i <= N; i++)   // updating Pi_bar
		{
			Pi_bar[i] = gamma[1][i];
		}

		for(i = 1; i <= N; i++)
		{
			for(j = 1; j <= N; j++)
			{
				sum1 = 0;
				sum2 = 0;
				for(t = 1; t <= T-1; t++)
				{
					sum1 = sum1 + Xi[t][i][j];   // calculating the numerator
					sum2 = sum2 + gamma[t][i];   // calculating the denominator
				}
				A_bar[i][j] = sum1/sum2;  // updating A_bar
			}
		}

		for(j = 1; j <= N; j++) 
		{
			for(k = 1; k <= M; k++)
			{
				sum1 = 0;
				sum2 = 0;
				for(t = 1; t <= T; t++)
				{
					if(O[obs_no][t] == k)
					{
						sum1 = sum1 + gamma[t][j];  // calculating the numerator
					}

					sum2 = sum2 + gamma[t][j];  // calculating the denominator
				}
				if(sum1 == 0)
					B_bar[j][k] = 1e-030;   // instead of 0's 1e-030 is inserted
				else
				B_bar[j][k]=sum1/sum2;    // updating B_bar
			}
		}


	}
	void update_lambda()  // updating A,B and Pi using A_bar,B_bar and Pi_bar obtained by the re-estimation procedure
	{
		int i = 0, j = 0, k = 0;
		int T = 85, N = 5, M = 32;
	
		for(i = 1; i <= N; i++)
		{
			Pi[i] = Pi_bar[i];

			for(j = 1; j <= N; j++)
				A[i][j] = A_bar[i][j];
			for(k = 1; k <= M; k++)
				B[i][k] = B_bar[i][k];
		}
	}
	void make_stochastic(int digit)   // this function makes sure that the converged matrices are stochastic
	{
		int i = 0, j = 0, k = 0, sum = 0, max = INT_MIN, index = 0;
		int T = 85, N = 5, M = 32;
	
	
		for(i = 1; i <= N; i++)
		{
			sum = sum + Pi_opt[digit][i];   // find sum of the row values
			if(max < Pi_opt[digit][i])
			{
				max = Pi_opt[digit][i];
				index = i;           // index stores the column index of the max element of that row
			}
		}

		if(sum > 1)   // we increase or decrease the required amount from the max element of that row
		{
			Pi_opt[digit][index] = Pi_opt[digit][index] - (sum-1);
		}
		else if(sum < 1)
		{
			Pi_opt[digit][index] = Pi_opt[digit][index] + (1-sum);
		}


		for(i = 1; i <= N; i++)
		{
			sum = 0;
			max = INT_MIN;
			index = 0;

			for(j = 1; j <= N; j++)
			{
				sum = sum + A_opt[digit][i][j];      // find sum of the row values
				if(max < A_opt[digit][i][j])
				{
					max = A_opt[digit][i][j];
					index = j;                       // index stores the column index of the max element of that row
				}
			}

			if(sum > 1)     // we increase or decrease the required amount from the max element of that row
			{
				A_opt[digit][i][index] = A_opt[digit][i][index] - (sum-1);
			}
			else if(sum < 1)
			{
				A_opt[digit][i][index] = A_opt[digit][i][index] + (1-sum);
			}
		}

		for(i = 1; i <= N; i++)
		{
			sum = 0;
			max = INT_MIN;
			index = 0;

			for(j = 1; j <= M; k++)
			{
				sum = sum + B_opt[digit][i][j];         // find sum of the row values
				if(max < B_opt[digit][i][j])
				{
					max = B_opt[digit][i][j]; 
					index = j;                   // index stores the column index of the max element of that row
				}
			}
			if(sum > 1)          // we increase or decrease the required amount from the max element of that row
			{
				B_opt[digit][i][index] = B_opt[digit][i][index] - (sum-1);
			}
			else if(sum < 1)
			{
				B_opt[digit][i][index] = B_opt[digit][i][index] + (1-sum);
			}
		}
	}
	void bakis_model()    // initializing A,B and Pi to the Bakis model
	{
		int T = 85, N = 5, M = 32;
		int i = 0, j = 0, k = 0;

		for(i = 1; i <= N; i++)
		{
			if(i == 1)
				Pi[i] = 1;
			else
			Pi[i] = 0;

			for(j = 1; j <= N; j++)
			{
				if(j == i)
					A[i][j] = 0.8;
				else if(j == i+1)
					A[i][j] = 0.2;
				else
				A[i][j] = 0;
			}
			for(k = 1; k <= M; k++)
				B[i][k] = 0.03125;
		}

		A[5][5] = 1;
	
	}
	void convergence(int digit, int obs_count, FILE* fp, FILE* fg)   // function to perform HMM convergence
	{
		int i = 0, j = 0, k = 0, m = 0, count = 1, obs_no = 1, loop = 1;
		int T = 85, N = 5, M = 32;
		long double p = 0, p_old = INT_MAX, p_star = 0;

		fprintf(fp,"For digit : %d\n\n\n",digit);

		load_obs_seq(digit,obs_count);   // first load the obs sequence of that digit

		while(loop <= 3)   // averaging 3 times
		{
			for(i = 1; i <= N; i++)  // Pi_avg, A_avg, B_avg will help in averaging the model after every iteration
			{
				Pi_avg[digit][i] = 0;  

				for(j = 1; j <= N; j++)
				{
					A_avg[digit][i][j] = 0;
				}
				for(k = 1; k <= M; k++)
				{	
					B_avg[digit][i][k] = 0;
				}
			}

			for(obs_no = 1; obs_no <= obs_count; obs_no++)   // for every observation sequence
			{
				count = 1;

				if(loop == 1)   // we initialize A,B and Pi to the bakis model
					bakis_model();
				else
				{
					for(i = 1; i <= N; i++)    // A_opt,B_opt and Pi_opt will store the converged models after every iteration
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
				}
	


				p_old = INT_MIN;
				int val = 1;
	
				while(count <= 200)   // max 200 iterations is set for convergence
				{
					forward(obs_no);
					backward(obs_no);
					p_star = viterbi(obs_no);
	
					if(p_star <= p_old)   // If there is no improvement in p_star value for 3 iterations then I stop the convergence procedure
					{
						if(val > 2)					
							break;
						val++;
					}
				
	
					BaumWelch(obs_no);
					re_estimation(obs_no);
					update_lambda();       // update A,B and Pi
	
					p_old = p_star;
	
					count++;
				}
	
			
				for(i = 1; i <= N; i++)    // finds the sum of all A,B and Pi matrices of all observations of the digit
				{
					Pi_avg[digit][i] = Pi_avg[digit][i] + Pi[i];
	
					for(j = 1; j <= N; j++)
						A_avg[digit][i][j] = A_avg[digit][i][j] + A[i][j];
					for(k = 1; k <= M; k++)
						B_avg[digit][i][k] = B_avg[digit][i][k] + B[i][k];
				}



				if(loop == 3)
				{
					fprintf(fp,"Observation sequence : %d\n\n",obs_no);

					//print the matrices into a text file

					fprintf(fp,"P* = %g\n\n",p_star);
				
					fprintf(fp,"Pi matrix: \n");
		
					for(i = 1; i <= N; i++)
					{
						fprintf(fp,"%g ",Pi[i]);
					
					}
					fprintf(fp,"\n\n");
					fprintf(fp,"A matrix: \n");
		
					for(i = 1; i <= N; i++)
					{
						for(j = 1; j <= N; j++)
						{
							fprintf(fp,"%g ",A[i][j]);
						}
						fprintf(fp,"\n");
					}
				
					fprintf(fp,"\n");
			
					fprintf(fp,"B matrix: \n");
				
					for(i = 1; i <= N; i++)
					{
						for(j = 1; j <= M; j++)
						{
							fprintf(fp,"%g ",B[i][j]);
						}

						fprintf(fp,"\n");
					}	
				
					fprintf(fp,"\n\n");
				}
	
	
			}
	

			for(i = 1; i <= N; i++)   // divide by obs_count to find the average and store it in A_opt,B_opt and Pi_opt
			{
				Pi_opt[digit][i] = Pi_avg[digit][i] / obs_count;
	
				for(j = 1; j <= N; j++)
				{
					A_opt[digit][i][j] = A_avg[digit][i][j] / obs_count;
				}
				for(k = 1; k <= M; k++)
					B_opt[digit][i][k] = B_avg[digit][i][k] / obs_count;
			}

		
			loop++;
		}


		//print the final converged matrices of the digit into a text file
	
	
		fprintf(fp,"Final converged Pi matrix: \n");
		
		for(i = 1; i <= N; i++)
		{
			fprintf(fp,"%g ",Pi_opt[digit][i]);
			fprintf(fg,"%g ",Pi_opt[digit][i]);
					
		}
		fprintf(fp,"\n\n");
		fprintf(fg,"\n");
		fprintf(fp,"Final converged A matrix: \n");
	
		for(i = 1; i <= N; i++)
		{
			for(j = 1; j <= N; j++)
			{
				fprintf(fp,"%g ",A_opt[digit][i][j]);
				fprintf(fg,"%g ",A_opt[digit][i][j]);
			}

			fprintf(fp,"\n");
			fprintf(fg,"\n");
		}

		fprintf(fp,"\n");

		fprintf(fp,"Final converged B matrix: \n");	
	
		for(i = 1; i <= N; i++)
		{
			for(j = 1; j <= M; j++)
			{
				fprintf(fp,"%g ",B_opt[digit][i][j]);
				fprintf(fg,"%g ",B_opt[digit][i][j]);
			}
			fprintf(fp,"\n");
			fprintf(fg,"\n");
		}	

		fprintf(fp,"\n\n");
		fprintf(fp,"----------------------------------------------------------------");
		fprintf(fp,"\n\n");

	}
	void record_new_word(int num, char roll[])  // function to record 20 utterances of a new word
	{
		int i = 0, j = 0, k = 1;
		char str[5];
		char dummy[30];
		char rollno[40];
	
		strcpy(rollno,roll);
		sprintf(str,"%d",num+1);
		strcat(rollno,"_");
		strcat(rollno,str);
		strcpy(dummy,rollno);
	
		while(k <= 20)
		{
			char sp[70];
			sprintf(str,"%d",k);
			strcat(dummy,"_");
			strcat(dummy,str);
			strcat(dummy,".txt");
	
			strcpy(sp,"Recording_Module.exe 3 live.wav ");
			strcat(sp,dummy);
			system(sp);
	
			strcpy(dummy,rollno);
	
			k++;
		}
	}

	void Training(int num_words)  // function to retrain the model with the new word
	{
		
		long double C[20][85][13] = {0};
		long double y[32][12] = {0};
		int num = 0, choice = 0;
		char roll[30] = "Speech/214101046";   // change this if you have different filenames and directory names
	
		int train_files = 20, i = 0, T = 85;
		char* universe = "Universe.txt";
		char str[5];
		FILE* fp;

		long double* X[25000];
		for (i = 0; i < 25000; i++)
			X[i] = (long double*)malloc(12 * sizeof(long double));

		fp = fopen(universe, "w");

		record_new_word(num_words,roll);
		num_words++;

		for(int i = 0; i <= num_words; i++)
		{
			sprintf(str,"%d",i);
			generate_Ci_digit(train_files,C,roll,str,0);      // first find the cepstral coefficients of all the train files

			for(int j = 0; j < train_files; j++)
			{
				for(int k = 0; k < T; k++)
				{
					for(int n = 1; n <= 12; n++)
					{
						fprintf(fp,"%Lf ",C[j][k][n]);
					}

					fprintf(fp,"\n");
				}
			}
		}

		fclose(fp);
	
		LBG(X,y,universe,train_files,num_words);    // generate codebook
		write_obs(X,y,train_files,num_words);       // generate observation sequences



		fp = fopen("output.txt", "w");
		FILE* fg = fopen("model1.txt","w");
	
		for(i = 0; i <= num_words; i++)
		{
			convergence(i,train_files,fp,fg);
		}
	
		fclose(fp);
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

