#!/bin/bash -x
set -e

log="$HOME/.windows.txt"
max_lines=50400 #60*24*7*5

find_common()
{
    string1="$1"
    string2="$2"
    if [[ ${#string1} -lt ${#string2} ]]
    then
            string1="$2"
            string2="$1"
    fi
    for ((i=${#string2}; i>0; i--)); do
        for ((j=0; j<=${#string2}-i; j++)); do
            if [[ $string1 =~ ${string2:j:i} ]]
            then
                echo ${string2:j:i}
                exit
            fi
        done
    done
}

truncate_log()
{
    if [[ -f "$log" ]]
    then
        lines=$(wc -l "$log" | awk '{print $1}')
        if [[ $lines -gt $max_lines ]]
        then
          to_delete=$((lines - max_lines))
          sed -i "1,${to_delete}d" "$log"
        fi
    fi
}

top_windows()
{
    truncate_log
    sed "s/-[^-]*$//" "$log" | sort | uniq -c | sort -rn | awk '{ $1 = $1 / 6; $1 = "$1 min"; print $0 }'

    exit 0
}

top_apps()
{
    truncate_log
    declare -a myarray
    readarray myarray < <(sed "s/.*- \([^-]*\)$/\1/" "$log" | sort | uniq -c | sort -rn | awk '{ $1 = $1 / 6; print $0}')

    # Explicitly report array content.
    for i in ${#myarray[@]}
    do
        echo "${myarray[3]}"
        for j in ${#myarray[@]}
        do
            echo "$j"
            common=$(find_common "$i" "$j")
            echo $common
        done
    done
}

monitor()
{
    while true
    do
        if [[ $(xprintidle) -lt 10000 ]] #sprawdzic czy nie vlc
        then
            xdotool getwindowfocus getwindowname >> "$log"
        fi

        sleep 10
    done
}

print_help()
{
cat <<-HERE
-a top apps
-w top windows
HERE
}

[[ "$1" == "-h" ]] && print_help

[[ "$1" == "-a" ]] && top_apps

[[ "$1" == "-w" ]] && top_windows

[[ "$#" -ne 0 ]] && exit 1

truncate_log
sleep 10
monitor &
