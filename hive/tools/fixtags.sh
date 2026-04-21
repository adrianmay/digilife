awk -F'\t' '{
  cmd = "tools/fixloc.sh \"" $2 "\""
  cmd | getline newval
  close(cmd)
  $2 = newval
  print
}' OFS='\t' tags > newtags
rm tags
mv newtags tags

