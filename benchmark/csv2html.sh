#!/bin/bash
echo "<html>" ;
echo "<head><style> table {border-collapse: collapse;} table, td, th {border: 1px solid black;} </style></head>"
echo "<title> REPORT </title>"
echo "<body><table border = '1' cellpadding=10>";
header=1
echo "<h1> REPORT </h1>"
IFS=','
while read -A line; do
    length=${#line[@]}
    if [[ "${length}" != "1" ]]; then
        echo "<tr>"
        for i in "${!line[@]}"; do
            value=${line[i]}
            if [[ "${header}" == "1" ]]; then
                echo "<th>${value}</th>"
            else
                echo "<td>${value}</td>"
            fi
        done
        echo "</tr>"
        header=0
    else
        echo "</table> <br/><br/> <table border = '1' cellpadding=15>"
        header=1
    fi
done < $1 ;
echo "</table></body></html>";