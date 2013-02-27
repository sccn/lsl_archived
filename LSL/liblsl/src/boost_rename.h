#ifndef BOOST_RENAME_H
#define BOOST_RENAME_H

// Including this header allows us to select between either the system-supplied boost location (<boost/...>) and namespace (boost::...)
// or to switch to a private location (<lslboost/...>, in the external folder) with renamed namespace (lslboost::...).
//
// This is done by instead of writing:
//   #include <boost/some_file.hpp>
// ... we write:
//   #include BOOST_PATH(/some_file.hpp)


// #define USE_OFFICIAL_BOOST   // uncomment this line if you intend to link against the official (e.g., operating system supplied) boost distribution

#define STRINGIZE(name) #name
#ifdef USE_OFFICIAL_BOOST
	// if we're using the system-supplied boost library, we use the default name
	#define BOOST_PATH(path) STRINGIZE(boost##path)
	#define BOOST_NAMESPACE boost
#else
	// if we're using the our private version, the location is called lslboost, which is in external/ and we add a namespace alias
	#define BOOST_PATH(path) STRINGIZE(lslboost##path)
	namespace lslboost { }
	namespace boost = lslboost;
	#define BOOST_NAMESPACE lslboost
#endif

#endif 