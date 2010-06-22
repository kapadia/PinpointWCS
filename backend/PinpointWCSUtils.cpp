/*
 *  PinpointWCS is developed by the Chandra X-ray Center
 *  Education and Public Outreach Group
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "PinpointWCSUtils.h"

namespace PinpointWCSUtils {
	
	float computeQuantile(float *arr, long arraysize, double q)
	{
		std::cout << "Determining Quantile ...\n";
		long length = arraysize;
		float dd;
		double dq;
		int i, s1len, s2len, randIndex;
		int num_below = 0;
		int iter = 0;
		float* D;
		float* s1;
		float* s2;
		float eps = 1. / length;
		
		// Initialize a random number
		srand(time(NULL));
		
		// Copy data to local variable
		D = (float *) malloc(length * sizeof(float));
		memcpy(D, arr, length * sizeof(float));
		
		while (true) {
			
			// Choose element from array
			randIndex = rand() % length;
			dd = D[randIndex];
			s1len = 0;
			s2len = 0;
			
			// Allot memory to the two arrays created
			s1 = (float *) malloc((length) * sizeof(float));
			s2 = (float *) malloc((length) * sizeof(float));
			
			// Begin comparison
			for (i=0; i<length; i++)
			{
				if (D[i] < dd)
				{
					s1[s1len] = D[i];
					s1len++;
				}
				else
				{
					s2[s2len] = D[i];
					s2len++;
				}
			}
			
			// Reallocate memory to both arrays
			s1 = (float *) realloc(s1, s1len * sizeof(float));
			s2 = (float *) realloc(s2, s2len * sizeof(float));
			
			// Check length of s1
			if (s1len == 0)
				return dd;
			
			// Determine the quantile of d
			dq = (double) (s1len + num_below) / arraysize;
			
			// Choose the appropriate array
			if (dq > q - eps and dq < q + eps)
				return dd;
			else if (dq < q)
			{
				// Choose s2
				D = (float *) realloc(D, s2len * sizeof(float));
				memcpy(D, s2, s2len * sizeof(float));
				length = s2len;
				num_below = num_below + s1len;
			}
			else
			{
				// Choose s1
				D = (float *) realloc(D, s1len * sizeof(float));
				memcpy(D, s1, s1len * sizeof(float));
				length = s1len;				
			}

			// Deallocate s1 and s2
			free(s1);
			free(s2);

//			std::cout << "dd = " << dd << "\n";
//			std::cout << "length = " << length << "\n";
//			std::cout << "dq = " << dq << "\n";
//			std::cout << "iter = " << iter << "\n\n";
			
			iter++;
		}  // while
		
	} // function
	
}  // namespace

