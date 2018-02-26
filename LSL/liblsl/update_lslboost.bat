set BOOSTPATH=C:\local\boost_1_66_0
REM not tested as thoroughly as update_lslboost.sh
rd /Q /s lslboost\boost
rd /Q /s lslboost\src
bcp --unix-lines --boost=%BOOSTPATH% --namespace=lslboost --scan lslboost/used_headers.h lslboost/
del lslboost\Jamroot
