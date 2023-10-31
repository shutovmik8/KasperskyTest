dd if=/dev/random of=tmp/in bs=1M count=1
./zip tmp/in tmp/out.zip
mv tmp/in tmp/in1
./unzip tmp/out.zip
if diff tmp/in tmp/in1; then
	echo $0 passed
fi
