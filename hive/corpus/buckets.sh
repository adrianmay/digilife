sed 's/^[^ ]* //' | awk '
{
    x[NR] = $1
}
END {
    min = max = x[1]
    for (i = 2; i <= NR; i++) {
        if (x[i] < min) min = x[i]
        if (x[i] > max) max = x[i]
    }

    n = 40
    width = (max - min) / n
    if (width == 0) {
        printf "%g | ", min
        for (i = 1; i <= NR; i++) printf "*"
        print ""
        exit
    }

    for (i = 1; i <= NR; i++) {
        b = int((x[i] - min) / width)
        if (b >= n) b = n - 1
        count[b]++
        if (count[b] > maxcount) maxcount = count[b]
    }

    for (b = 0; b < n; b++) {
        lo = min + b * width
        hi = lo + width
        printf "%10.4g - %-10.4g | ", lo, hi
        for (j = 0; j < count[b]; j++) printf "*"
        print ""
    }
}' 
