#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

using namespace std;

// structure to store all joint angle data
struct skeleton{
	double first_angle;
	double second_angle;
	double third_angle;
	double fourth_angle;
	double fifth_angle;
	double sixth_angle;
	int cluster;

	skeleton(){};
};

// function to calculate the difference between two skeletons
double cal_Distance(struct skeleton s1, struct  skeleton s2){
	double first_dist, second_dist, third_dist, fourth_dist, fifth_dist, sixth_dist;

	first_dist = pow(fabs(s1.first_angle - s2.first_angle), 2);
	second_dist = pow(fabs(s1.second_angle - s2.second_angle), 2);
	third_dist = pow(fabs(s1.third_angle - s2.third_angle), 2);
	fourth_dist = pow(fabs(s1.fourth_angle - s2.fourth_angle), 2);
	fifth_dist = pow(fabs(s1.fifth_angle - s2.fifth_angle), 2);
	sixth_dist = pow(fabs(s1.sixth_angle - s2.sixth_angle), 2);

	return sqrt(first_dist + second_dist + third_dist + fourth_dist + fifth_dist + sixth_dist);
}

int main(){

	skeleton cat1, cat2;

	vector<skeleton> skeletons;
	char *fileName =  "raw.data";

	FILE *file = fopen(fileName, "r");
	if(!file){
		printf("could not open '%s'\n", fileName);
		return -1;
	}
	printf("successful in openning '%s'\n", fileName);

	// buffer with 80 char space to read file
	char buffer[80];
	while(fgets(buffer, sizeof(buffer), file)){
		int dummy;
		struct skeleton skl;

		// sscanf returns the number of items successfully scanned
		int r = sscanf(buffer, "%lf , %lf , %lf , %lf , %lf , %lf , %d", 
					   &skl.first_angle, 
					   &skl.second_angle,
					   &skl.third_angle,
					   &skl.fourth_angle,
					   &skl.fifth_angle,
					   &skl.sixth_angle,
					   &dummy);
		if(r == 7){
			printf("successful in scanning data\n");
			skeletons.push_back(skl);
		}else{
			printf("sscanf returned %d on '%s'\n", buffer);
		}
	}

	fclose(file);

	// create 2 randome starting seeds
	cat1.first_angle = 180*(rand()/(double)RAND_MAX);
	cat1.second_angle = 180*(rand()/(double)RAND_MAX);
	cat1.third_angle = 180*(rand()/(double)RAND_MAX);
	cat1.fourth_angle = 180*(rand()/(double)RAND_MAX);
	cat1.fifth_angle = 180*(rand()/(double)RAND_MAX);
	cat1.sixth_angle = 180*(rand()/(double)RAND_MAX);

	cat2.first_angle = 180*(rand()/(double)RAND_MAX);
	cat2.second_angle = 180*(rand()/(double)RAND_MAX);
	cat2.third_angle = 180*(rand()/(double)RAND_MAX);
	cat2.fourth_angle = 180*(rand()/(double)RAND_MAX);
	cat2.fifth_angle = 180*(rand()/(double)RAND_MAX);
	cat2.sixth_angle = 180*(rand()/(double)RAND_MAX);

	// initialize distance vector 
	// (2D array listing the distance between every datum and each classification)
	vector<vector<double> > differences;
	for(int i = 0; i < skeletons.size(); i++) {
		differences.push_back(vector<double>(2));
	}

	// counting the number of datums being classified to each categories
	int numOfCat1, numOfCat2;

	int numOfIterations = 0;

	// loop
	while(1) {
		numOfCat1 = 0;
		numOfCat2 = 0;

		// classify all datum based on current centre points
		for(int i = 0; i < skeletons.size(); i++) {
			if(cal_Distance(skeletons[i], cat1) > cal_Distance(skeletons[i], cat2)) {
				skeletons[i].cluster = 2;
				numOfCat2 += 1;
			} else {
				skeletons[i].cluster = 1;
				numOfCat1 += 1;
			}
		}

		// clear average values
		cat1.first_angle = 0;
		cat1.second_angle = 0;
		cat1.third_angle = 0;
		cat1.fourth_angle = 0;
		cat1.fifth_angle = 0;
		cat1.sixth_angle = 0;

		cat2.first_angle = 0;
		cat2.second_angle = 0;
		cat2.third_angle = 0;
		cat2.fourth_angle = 0;
		cat2.fifth_angle = 0;
		cat2.sixth_angle = 0;

		// using new classification to update centre points
		for(int i = 0; i < skeletons.size(); i++) {
			if(skeletons[i].cluster == 1) {
				cat1.first_angle += skeletons[i].first_angle / numOfCat1;
				cat1.second_angle += skeletons[i].second_angle / numOfCat1;
				cat1.third_angle += skeletons[i].third_angle / numOfCat1;
				cat1.fourth_angle += skeletons[i].fourth_angle / numOfCat1;
				cat1.fifth_angle += skeletons[i].fifth_angle / numOfCat1;
				cat1.sixth_angle += skeletons[i].sixth_angle / numOfCat1;
			} else {
				cat2.first_angle += skeletons[i].first_angle / numOfCat2;
				cat2.second_angle += skeletons[i].second_angle / numOfCat2;
				cat2.third_angle += skeletons[i].third_angle / numOfCat2;
				cat2.fourth_angle += skeletons[i].fourth_angle / numOfCat2;
				cat2.fifth_angle += skeletons[i].fifth_angle / numOfCat2;
				cat1.sixth_angle += skeletons[i].sixth_angle / numOfCat2;
			}
		}

		numOfIterations += 1;

		if(numOfIterations == 300) {
			break;
		}
	}	

	file = fopen("clustered.data", "w");

	for(int i = 0; i < skeletons.size(); i++) {
		fprintf(file, "%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %d \n", 
				skeletons[i].first_angle, skeletons[i].second_angle, skeletons[i].third_angle,
				skeletons[i].fourth_angle, skeletons[i].fifth_angle, skeletons[i].sixth_angle,
				skeletons[i].cluster);
	}

	fclose(file);

	printf("k means learning process has successfully completed.\n");

	printf("cat1 is: %lf %lf %lf %lf %lf %lf \n", 
			cat1.first_angle, cat1.second_angle, cat1.third_angle,
			cat1.fourth_angle, cat1.fifth_angle, cat1.sixth_angle);
	return 1;
}