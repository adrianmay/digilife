awk -F'\t' '{ 
  if ($2 ~ /^gen\/[^_/]*\//)          gsub(/^gen/,         "lit", $2);
  if ($2 ~ /^gen\/[^_/.]*\..$/)       gsub(/^gen/,         "lit", $2);
  if ($2 ~ /^gen\/[^./_]*_[^./_]*\//) gsub(/^gen\/[^_]*_/, "tpt/", $2);
  gsub(/^bin\/test\//, "test/", $2);
  gsub(/^bin\/hive\//, "hive/", $2);
  print
}' OFS='\t' tags > newtags
