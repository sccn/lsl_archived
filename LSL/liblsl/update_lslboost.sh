BOOSTPATH=/tmp/boost_1_66_0
# to update this, use boost bcp as follows (on windows, you need to install a unix shell):
rm -rf lslboost/{boost,libs}
bcp --unix-lines --boost=$BOOSTPATH --namespace=lslboost --scan `find src -regex ".+\.[ch]p*"` lslboost/additional_headers.h lslboost/
rm lslboost/Jamroot
# optionally: remove superfluous directories:
find lslboost -type d -and \( -name build -o -name test -o -name edg -o -name dmc -o -name msvc70 -o -name msvc60 -o -name bcc* \) -print0 | xargs -0 rm -rf

# apply patches to boost
# remove minimum wait time
patch -p3 < lslboost/boost-thread-windows.patch
