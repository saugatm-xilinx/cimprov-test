#!/bin/bash

if test $# -gt 0 ; then
    files=$(find $1 -name "*.dat")
else
    files=
fi

echo "Including firmware images:"
echo "$files"

out=./fw_images.c
i=0

echo "#include \"fw_images.h\"" >$out
echo "" >>$out
for f in $files
do
    echo "uint8_t img${i}[] __attribute__((aligned(4))) = {" >>$out
    hexdump "$f" -v -e '/1 "0x%02X, "' >>$out
    echo "};" >>$out
    echo "" >>$out
    i=$(($i + 1))
done

echo "const fw_img_descr firmware_images[] = {" >>$out
for (( j = 0; j < $i; j++ ))
do
    echo "    { img$j, sizeof(img$j) }," >>$out
done
echo "};" >>$out
echo "" >>$out

echo "unsigned int fw_images_count = $i;" >>$out
