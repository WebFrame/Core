#!/bin/bash

rm tmp/test1.result;

for i in `seq 1 30`; do
    echo -n "$i ";
    (
        echo -en "C++-O\t" ; 
        curl -w "%{time_total}" http://localhost:8888/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O1\t" ;
        curl -w "%{time_total}" http://localhost:8889/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O2\t" ;
        curl -w "%{time_total}" http://localhost:8890/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O3\t" ;
        curl -w "%{time_total}" http://localhost:8891/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Ofast\t" ;
        curl -w "%{time_total}" http://localhost:8892/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Og\t" ;
        curl -w "%{time_total}" http://localhost:88893/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Os\t" ;
        curl -w "%{time_total}" http://localhost:8894/$i -o /dev/null 2> /dev/null ;
        echo -en "\tPython\t" ;
        curl -w "%{time_total}" http://localhost:5000/$i -o /dev/null 2> /dev/null ;
        echo -en "\tNode.JS Express\t" ;
        curl -w "%{time_total}" http://localhost:3000/$i -o /dev/null 2> /dev/null
    ) | sed ':a;N;$!ba;s/\n/ /g' >> tmp/test1.result
    echo "" >> tmp/test1.result
done

rm tmp/test2.result;

for i in `seq 1 30`; do
    echo -n "$i ";
    (
        echo -en "C++-O\t" ; 
        curl -w "%{time_total}" http://localhost:8888/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O1\t" ;
        curl -w "%{time_total}" http://localhost:8889/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O2\t" ;
        curl -w "%{time_total}" http://localhost:8890/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-O3\t" ;
        curl -w "%{time_total}" http://localhost:8891/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Ofast\t" ;
        curl -w "%{time_total}" http://localhost:8892/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Og\t" ;
        curl -w "%{time_total}" http://localhost:88893/$i -o /dev/null 2> /dev/null ;
        echo -en "\tC++-Os\t" ;
        curl -w "%{time_total}" http://localhost:8894/$i -o /dev/null 2> /dev/null ;
        echo -en "\tPython\t" ;
        curl -w "%{time_total}" http://localhost:5000/$i -o /dev/null 2> /dev/null ;
        echo -en "\tNode.JS Express\t" ;
        curl -w "%{time_total}" http://localhost:3000/$i -o /dev/null 2> /dev/null
    ) | sed ':a;N;$!ba;s/\n/ /g' >> tmp/test2.result
    echo "" >> tmp/test2.result
done