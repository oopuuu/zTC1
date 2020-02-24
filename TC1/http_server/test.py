#! /bin/env python3

import os
import glob
import binascii
import gzip
import re

try:
    io = __import__("io").BytesIO
except:
    io = __import__("StringIO").StringIO

def gen(dat, fn):
    try:
        s = ','.join(["0x%02x" % c for c in dat])
    except:
        s = ','.join(["0x"+binascii.hexlify(c) for c in dat])

    s = re.sub("((?:0x.+?,){16})", "\\1\n", s)

    fn = re.sub(r"[^\w]", "_", fn)
    print("const unsigned char %s[0x%x] = {\n%s};" % (fn, len(dat), s))

def gz_gen(s, fn):
    dat = io()
    with gzip.GzipFile(fileobj=dat, mode="w") as f:
        f.write(s)
    dat = dat.getvalue()
    gen(dat, fn)

def pack(path, name):
    s = b''
    for fn in glob.glob(path):
        s += ("/*%s*/\n" % fn).encode('utf-8')
        s += open(fn, 'rb').read()
    gz_gen(s, name)
    
pack('web/*.js', 'js_pack')
pack('web/*.css', 'css_pack')

for fn in glob.glob('web/*.html'):
    gz_gen(open(fn, 'rb').read(), fn)

for fn in glob.glob('web/*.woff2'):
    gen(open(fn, 'rb').read(), fn)