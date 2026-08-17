awk '{
    for (i = 1; i < length($0); i++) {
        p = substr($0,i,2)
        c[p]++
    }
}
END {
    for (p in c)
        print p, c[p]
}' | sort
