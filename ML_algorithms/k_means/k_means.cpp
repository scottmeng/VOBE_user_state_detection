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
			printf("successful in scanning data");
			skeletons.push_back(skl);
		}else{
			printf("sscanf returned %d on '%s'\n", buffer);
		}
	}

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

	double aveFirst1, aveSecond1, aveThird1, aveFourth1, aveFifth1, aveSixth1,
		   aveFirst2, aveSecond2, aveThird2, aveFourth2, aveFifth2. aveSixth2;

	// loop
	while(1) {
		// classify all datum based on current centre points
		for(int i = 0; i < skeletons.size(); i++) {
			if(cal_Distance(skeletons[i], cat1) > cal_Distance(skeletons[i], cat2)) {
				skeletons[i].cluster = 2;
			} else {
				skeletons[i].cluster = 1;
			}
		}

		// using new classification to update centre points
		for(int i = 0; i < skeletons.size(); i++) {
			if(skeletons[i].cluster == 1) {
				
			} else {
				
			}
		}
	}	


	return 1;
}