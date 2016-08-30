//General utility functions.
//Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD, 2011

#include "LinearAlgebra.h"
#include<vcl.h>
#pragma hdrstop
#include<list.h>
#include "general_util.h"

//---------------------------------------------------------------------------

#ifndef math_utilH
#define math_utilH
//---------------------------------------------------------------------------

#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INF
#define INF (1.0/0.0)
#endif

#define PI 3.1415926535897932384626433832795



//use as array2D[width][height]
template<typename T> T ** new2D(int width, int height, T initialValue) {
	T **t2D = new T*[width];
	for(int x = 0; x < width; x++) {
		t2D[x] = new1D<T>(height, initialValue);
	}
	return t2D;
}


//frees  array2D[width][height]
template<typename T> void delete2D(T ** array2D, int width) {
	if(array2D) {
		for(int i=0; i< width; i++) {
			delete1D(array2D[i]);
		}
		delete1D(array2D);
	}

}

//use as array[length]
template<typename T> T * new1D(int length, T initialValue) {
	T *t1D = new T[length];
	for(int i=0; i<length; ++i) {
		t1D[i] = initialValue;
	}
	return t1D;
}

template<typename T> void delete1D(T * array) {
	if(array) delete[] array;
	array = NULL;
}

//use as
//calculateStats(data, dataLength &mean, &stddev);
template<typename T> void calculateStats(T *data, int dataLength, double *mean, double *stddev) {
	double sum = 0.0;
	double sumSq = 0.0;
	double n = (double) dataLength;
	for(int i=0; i<dataLength; i++) {
		sum += data[i];
		sumSq += data[i]*data[i];
	}
	if(_isnan(sum)) {
		*mean =  NAN;
		*stddev = NAN;
		return;
	}
	*mean = (n > 0.0) ? sum/n : NAN;
	if (n < 2.0) {
		*stddev = NAN;
	} else {
		//prevent floating point error from causing sqrt of negative
		double temp = (sumSq - sum*sum/n)/(n-1);
		if (temp < 0.0) *stddev = 0.0;
		else *stddev = sqrt(temp);
	}
}

//use as
//list<T> data;
//double mean, stddev;
//calculateStats(data, &mean, &stddev);
template<typename T> void calculateStats(list<T>& data, double *mean, double *stddev) {
	double sum = 0.0;
	double sumSq = 0.0;
	double n = 0.0;
	list<T>::iterator i;
	for(i=data.begin(); i != data.end(); ++i) {
		sum += (double) *i;
		sumSq += (double) (*i)*(*i);
		++n;
	}
	if(_isnan(sum)) {
		*mean =  NAN;
		*stddev = NAN;
		return;
	}
	*mean = (n > 0.0) ? sum/n : NAN;
	if (n < 2.0) {
		*stddev = NAN;
	} else {
		//prevent floating point error from causing sqrt of negative
		double temp = (sumSq - sum*sum/n)/(n-1);
		if (temp < 0.0) *stddev = 0.0;
		else *stddev = sqrt(temp);
	}


}

//use as
//vector<T> data;
//double mean, stddev;
//calculateStats(data, &mean, &stddev);
template<typename T> void calculateStats(vector<T>& data, double *mean, double *stddev) {
	double sum = 0.0;
	double sumSq = 0.0;
	double n = 0.0;
	vector<T>::iterator i;
	for(i=data.begin(); i != data.end(); ++i) {
		sum += (double) *i;
		sumSq += (double) (*i)*(*i);
		++n;
	}
	if(_isnan(sum)) {
		*mean =  NAN;
		*stddev = NAN;
		return;
	}
	*mean = (n > 0.0) ? sum/n : NAN;
	if (n < 2.0) {
		*stddev = NAN;
	} else {
		//prevent floating point error from causing sqrt of negative
		double temp = (sumSq - sum*sum/n)/(n-1);
		if (temp < 0.0) *stddev = 0.0;
		else *stddev = sqrt(temp);
	}


}

template<typename T> ublas::matrix<T> calculateCovariance2D(T *dataX, T *dataY, int dataLength) {

	ublas::matrix<T> covariance(2,2);
	if(dataLength < 2) {
		covariance *= NAN;
		return covariance;
	}
	covariance *= 0;

	double sumX = 0.0;
	double sumY = 0.0;
	double sumXX = 0.0;
	double sumYY = 0.0;
	double sumXY = 0.0;

	for(int i=0; i<dataLength; i++) {
		sumX += dataX[i];
		sumY += dataY[i];
		sumXX += dataX[i]*dataX[i];
		sumXY += dataX[i]*dataY[i];
		sumYY += dataY[i]*dataY[i];
	}


	if(_isnan(sumX) || _isnan(sumY) || _isnan(sumXX) || _isnan(sumXY) || _isnan(sumYY)) {
		covariance *= NAN;
		return covariance;

	}

	covariance(0,0) = (sumXX - sumX * sumX / dataLength)/(dataLength - 1);
	covariance(1,1) = (sumYY - sumY * sumY / dataLength)/(dataLength - 1);
	covariance(0,1) = (sumXY - sumX * sumY / dataLength)/(dataLength - 1);
	covariance(1,0) = covariance(0,1);

	return covariance;
}

/*
  1 3 3 3
  2 3 3 3
  3 3 3 3
  becomes UnicodeString(1,2,3;3,3,3;3,3,3;3,3,3)
  note that this is column major, the transpose of matlab
*/
template<typename T> UnicodeString matrixToUnicodeString(ublas::matrix<T> m) {

	UnicodeString result = "";

	for(unsigned int col=0; col<m.size2(); col++) {
		for(unsigned int row=0; row<m.size1(); row++) {
			if(row < m.size1()-1)
				result = result + UnicodeString(m(row,col)) + ",";
			else if (col < m.size2()-1)
				result = result + UnicodeString(m(row,col)) + ";";
			else
				result = result + UnicodeString(m(row,col));
		}
	}
	return result;
}

/*
 3 3 3
 becomes UnicodeString(3,3,3)
*/
template<typename T> UnicodeString vectorToUnicodeString(ublas::vector<T> v) {
	UnicodeString result = "";
	for(unsigned int i=0; i<v.size(); i++) {
		if(i<v.size()-1)
			result = result + UnicodeString(v(i)) + ",";
		else
			result = result + UnicodeString(v(i));
	}
	return result;
}



template<typename T> ublas::vector<T> unicodeStringToVector(UnicodeString string) {
	std::vector<UnicodeString> strs = splitString(string, L',');
	ublas::vector<T> result(strs.size(), 0.0);
	for(unsigned int i=0; i<strs.size(); i++) {
		result(i) = (T) strs[i].ToDouble();
	}
	return result;
}

template<typename T> ublas::matrix<T> unicodeStringToMatrix(UnicodeString string) {

	std::vector<UnicodeString> colStrings = splitString(string, L';');

	std::vector<UnicodeString> elementStrings = splitString(colStrings[0], L',');

	ublas::matrix<T> result(elementStrings.size(),colStrings.size());
	for(unsigned int col = 0; col<colStrings.size(); col++) {
		elementStrings = splitString(colStrings[col],L',');
		if(elementStrings.size() != result.size1()) throw IllegalArgumentException("unicodeStringToMatrix: columns are not all the same length.");
		for(unsigned int row = 0; row<elementStrings.size(); row++) {
			result(row, col) = (T) elementStrings[row].ToDouble();
		}
	}
	return result;
}


#endif

