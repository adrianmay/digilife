tr -d '\n' | awk '{
    for (i=1; i<=length($0)-1; i++) {
        p=substr($0,i,2)
        c[p]++
        n[substr(p,1,1)]++
    }
}
END {
    for (p in c)
        printf "%s %.8f\n", p, c[p]/n[substr(p,1,1)]
}' | sort
