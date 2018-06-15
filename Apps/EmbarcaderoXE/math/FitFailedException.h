//---------------------------------------------------------------------------

#ifndef FitFailedExceptionH
#define FitFailedExceptionH
//---------------------------------------------------------------------------
#include "exception"

class FitFailedException : public std::exception {

	public:
		char * description;
		FitFailedException(char * description) {
			this->description = description;
        }
	virtual const char* what() const throw()
	{
		return description;
	}
};
#endif
