#!/bin/bash
#wget http://www.iozone.org/src/current/iozone3_22.tar
url='http://www.iozone.org/src/current/'
#tmpdir=`mktemp -dt  iozone.XXXXXX`
tmpdir="/tmp/iozone.oVDVJI"
echo tmpdir $tmpdir
answer=''
for i in {22..471};do
    echo $i
    bn=iozone3_$i
    fn=${url}${bn}.tar
    echo $fn
    if [ ! -f $tmpdir/$bn.tar ];then
        wget -c  -t 0  $fn -O $tmpdir/$bn.tar
    fi
    if [ $? -ne 0 ];then
        continue
    fi
    tar xvf $tmpdir/$bn.tar -C  $tmpdir/

    if [ $? -ne 0 ];then
        echo download or archive $bn.tar fail
        continue
    fi

    extractdir=$tmpdir/$bn
    if [ ! -d $extractdir ];then
        extractdir=$tmpdir
    fi

    files=(*)
    dfns=${files[@]/autoget.sh/}
    echo $dfns
    rm -rf $dfns
    mv $extractdir/src/current/* .
    mv $extractdir/docs/ .
    git add -A   .
    msg="`git diff --exit-code --cached --ignore-all-space --ignore-blank-lines  --no-color  Changes.txt \
        | sed  -e '/^\+.*/!d' -e '/^\+\{3\}/d' -e 's/^\+\=.*//g'  -e 's/\+//g' | sed -e '1{/^$/d}' -e '${/^$/d}' `"
    git diff --exit-code --cached --quiet
    if [ $? -ne  0 ] ;then
        if [ "$msg" == "" ];then
            msg="Revision 3.$i"
            #read answer
        fi
        git commit -m "$msg"
    else
        echo no diff $bn.tar
        read answer
    fi
done
