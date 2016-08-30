
//---------------------------------------------------------------------------

#ifndef CalculationFailedExceptionH
#define CalculationFailedExceptionH
//---------------------------------------------------------------------------
#include "exception"

class CalculationFailedException : public std::exception {

	public:
		char * description;
		CalculationFailedException(char * description) {
			this->description = description;
        }
	virtual const char* what() const throw()
	{
		return description;
	}
};
#endif