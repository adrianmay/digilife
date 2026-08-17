awk '{
    for (i = 1; i <= length($0); i++) c[substr($0,i,1)]++
    n += length($0)
}
END {
    for (i = 97; i <= 115; i++) {
        ch = sprintf("%c", i)
        p = c[ch] / n
        printf "%.10f %s\n", p, ch
    }
}' | sort -k1,1nr | awk '{
    printf "%s %8.5f | ", $2, $1
    for (i = 0; i < $1 * 1000; i++) printf "*"
    print ""
}'
