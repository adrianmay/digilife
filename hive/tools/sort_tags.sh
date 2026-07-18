mv tags gats
cat gats | grep -Pv "\tf\t" > tags
cat gats | grep -P "\tf\t" >> tags
rm gats
