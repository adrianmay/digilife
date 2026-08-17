tr -d '\n' | awk '{
    for (i=1; i<=length($0)-2; i++) {
        t=substr($0,i,3)
        c[t]++
        n[substr(t,1,2)]++
    }
}
END {
    for (t in c)
        printf "%s %.8f\n", t, c[t]/n[substr(t,1,2)]
}' | sort
