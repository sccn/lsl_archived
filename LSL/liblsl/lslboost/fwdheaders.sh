#/bin/sh

for dir  in `grep " <lslboost" -hr ../src/ | sed -e 's/#include <//;s/[^/]*.hpp>$//'|sort|uniq`; do mkdir -p $dir; done
for file in `grep " <lslboost" -hr ../src/ | sed -e 's/#include <lsl//;s/>$//'|sort|uniq`; do echo "#include <lsl$f>" > lsl$f; done
