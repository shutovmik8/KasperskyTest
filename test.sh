cd tests
for x in *.sh; do 
	(cd ../;mkdir tmp; bash tests/$x; rm -r tmp)
done	
