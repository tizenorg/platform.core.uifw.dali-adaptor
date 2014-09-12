#!/bin/bash

( cd ../build/tizen ; make cov_data )

# From lcov version 1.10 onwards, branch coverage is off by default and earlier versions do not support the rc option
LCOV_OPTS=`echo | awk -v version=\`printf "\\\`lcov --version | sed 's/^.* //g'\\\`\n1.10\n" | sort -V | head -n 1\` '{if ( version == 1.10 ) printf("--rc lcov_branch_coverage=1\n", version);}'`

for i in `find . -name "*.dir"` ; do
    (
        cd $i
        echo `pwd`
        covs=( `ls *.gcda 2>/dev/null` )
        if [[ $? -eq 0 ]]
        then
            lcov $LCOV_OPTS --directory . -c -o dali.info
            lcov $LCOV_OPTS --remove dali.info "*boost*" "/usr/include/*" "*/automated-tests/*" -o dali.info
        fi
    )
done

(
    cd .. ;
    genhtml $LCOV_OPTS -o build/tizen/doc/coverage `find . -name dali.info`
)

echo "Coverage output: ../build/tizen/doc/coverage/index.html"
