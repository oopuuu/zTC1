version_file=".version"
if [ ! -f $version_file ]; then echo "v1.0.0" > $version_file; fi

# 读当前版本并自增
ver=$(cat $version_file)
major=$(echo $ver | cut -d. -f1 | sed 's/v//')
minor=$(echo $ver | cut -d. -f2)
patch=$(echo $ver | cut -d. -f3)
patch=$((patch + 1))
new_ver="v${major}.${minor}.${patch}"
echo $new_ver > $version_file


cd ./TC1/http_server

python ./test.py > web_data.c

cd ../..

mico make TC1@MK3031@moc total

cp ./build/TC1\@MK3031\@moc/binary/TC1\@MK3031\@moc.ota.bin ./build/TC1\@MK3031\@moc/binary/ota.bin

cp ./build/TC1\@MK3031\@moc/binary/TC1\@MK3031\@moc.ota.bin ./build/TC1\@MK3031\@moc/binary/ota$(date +%Y%m%d%H%M%S).bin

cp ./build/TC1\@MK3031\@moc/binary/TC1\@MK3031\@moc.all.bin ./build/TC1\@MK3031\@moc/binary/all$(date +%Y%m%d%H%M%S).bin

