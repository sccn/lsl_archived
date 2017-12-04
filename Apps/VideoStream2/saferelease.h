#ifndef SAFERELEASE_H
#define SAFERELEASE_H

// standard windows templated release for wmf stuff
template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
#endif // SAFERELEASE_H