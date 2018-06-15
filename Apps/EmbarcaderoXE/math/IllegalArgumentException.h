//---------------------------------------------------------------------------

#ifndef IllegalArgumentExceptionH
#define IllegalArgumentExceptionH

#include "exception"

class IllegalArgumentException : public std::exception {

	public:
		char * description;
		IllegalArgumentException(char * description) {
			this->description = description;
		}
	virtual const char* what() const throw()
	{
		return description;
	}
};

//---------------------------------------------------------------------------
#endif
