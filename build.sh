cd ./TC1/http_server

python ./test.py > web_data.c

cd ../..

mico make clean

mico make TC1@MK3031@moc total

cp ./build/TC1\@MK3031\@moc/binary/TC1\@MK3031\@moc.ota.bin  ./build/TC1\@MK3031\@moc/binary/ota.bin
