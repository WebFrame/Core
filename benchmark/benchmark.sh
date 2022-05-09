#!/bin/bash

chmod +x ./csv2html.sh

declare -r n=25

for url in "1" "2"; do
    rm tmp/test$url.curl.csv;
    rm tmp/test$url.time.csv;
    echo -e "C++-O,C++-O1,C++-O2,C++-O3,C++-Ofast,C++-Og,C++-Os,C++-O-atomic,C++-O1-atomic,C++-O2-atomic,C++-O3-atomic,C++-Ofast-atomic,C++-Og-atomic,C++-Os-atomic,Python,Node.JS Express" > tmp/test$url.curl.csv;
    echo -e "C++-O,C++-O1,C++-O2,C++-O3,C++-Ofast,C++-Og,C++-Os,C++-O-atomic,C++-O1-atomic,C++-O2-atomic,C++-O3-atomic,C++-Ofast-atomic,C++-Og-atomic,C++-Os-atomic,Python,Node.JS Express" > tmp/test$url.time.csv;
    for i in `seq 0 $n`; do
        echo -n "$i ";
        ( 
            timeout 3m curl -w "%{time_total}" http://localhost:8888/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8889/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8890/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8891/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8892/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8893/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8894/$i/$url -o /dev/null --silent ; echo -en "," ; # asm("")
            timeout 3m curl -w "%{time_total}" http://localhost:8895/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8896/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8897/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8898/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8899/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8900/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 3m curl -w "%{time_total}" http://localhost:8901/$i/$url -o /dev/null --silent ; echo -en "," ; # using atomic variable
            timeout 20m curl -w "%{time_total}" http://localhost:5000/$i/$url -o /dev/null --silent ; echo -en "," ;
            timeout 10m curl -w "%{time_total}" http://localhost:3000/$i/$url -o /dev/null --silent
        ) | sed ':a;N;$!ba;s/\n/ /g' >> tmp/test$url.curl.csv
        echo "" >> tmp/test$url.curl.csv
        
        echo -n "$i ";
        ( 
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8888/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8889/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8890/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8891/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8892/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8893/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8894/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # asm("")
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8895/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8896/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8897/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8898/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8899/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8900/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 3m curl http://localhost:8901/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ; # using atomic variable
            /usr/bin/time -f "%e" timeout 20m curl http://localhost:5000/$i/$url -o /dev/null --silent 2>&1 ; echo -en "," ;
            /usr/bin/time -f "%e" timeout 10m curl http://localhost:3000/$i/$url -o /dev/null --silent 2>&1 ; 
        ) | sed ':a;N;$!ba;s/\n/ /g' >> tmp/test$url.time.csv
    done
    ./csv2html.sh tmp/test$url.curl.csv "Test $url / CURL total time" > tmp/test$url.curl.html
    ./csv2html.sh tmp/test$url.time.csv "Test $url / CURL time %e" > tmp/test$url.time.html
done

rm tmp/index.html;

for i in tmp/*.*.html; do
    cat $i >> tmp/index.html; 
done
