BOOSTPATH=/tmp/boost_1_65_1
# to update this, use boost bcp as follows (on windows, you need to install a unix shell):
rm -rf lslboost/{boost,src}
bcp --unix-lines --boost=$BOOSTPATH --namespace=lslboost --scan `find src -regex ".+\.[ch]p*"` lslboost/additional_headers.h lslboost/
rm lslboost/Jamroot
# optionally: remove superfluous directories:
find lslboost -type d -name build -or -name test -print0 | xargs -0 rm -rf

