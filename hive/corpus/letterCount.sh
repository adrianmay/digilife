od -An -v -tu1 | tr -s ' ' '\n' |
awk '{ c[$1]++ } END {
    for (i = 0; i < 256; i++)
        if (c[i]) {
            ch = (i >= 33 && i <= 126) ? sprintf("%c", i) : " "
            printf " %.2X %8d  %s\n", i, c[i], ch
          }
}'
