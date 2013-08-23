# file name: git_local_ver_build.sh

#!/bin/bash 
VER_FILE=GitVersion.h

GIT_VERSION=`git rev-list HEAD | wc -l | awk '{print $1}'`
echo "GIT_VERSION:" $GIT_VERSION

GIT_HASH_VERSION="$(git rev-list HEAD -n 1)"
echo "GIT_HASH_VERSION:" $GIT_VERSION

echo "Generated" $VER_FILE 

echo "#define GIT_VERSION $GIT_VERSION" > $VER_FILE
echo "#define GIT_HASH_VERSION $GIT_HASH_VERSION" >> $VER_FILE

echo "Job done!!"