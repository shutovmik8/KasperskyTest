dd if=/dev/random of=tmp/in bs=1M count=1
echo mypassword | ./zip -p tmp/in tmp/out.zip
mv tmp/in tmp/in1
echo mypassword | ./unzip tmp/out.zip
if diff tmp/in tmp/in1; then
	echo $0 passed
fi
