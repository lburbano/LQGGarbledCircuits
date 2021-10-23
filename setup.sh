#!/bin/bash
mkdir tmp
cd tmp

echo "Installing emp-toolkit"
wget https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/scripts/install.py
python3 install.py -install -tool -sh2pc
echo "Done installing emp-toolkit"

cd ..
rm -rf tmp
