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

def gen(fn):
    s = open(fn, 'rb').read()
    dat = io()
    with gzip.GzipFile(fileobj=dat, mode="w") as f:
        f.write(s)
    dat = dat.getvalue()
    try:
        s = ','.join(["0x%02x" % c for c in dat])
    except:
        s = ','.join(["0x"+binascii.hexlify(c) for c in dat])

    s = re.sub("((?:0x.+?,){16})", "\\1\n", s)

    fn = fn.replace('.', '_')
    print("const unsigned char %s[0x%x] = {\n%s};" % (fn, len(dat), s))

for fn in glob.glob('*.html'):
    gen(fn)

for fn in glob.glob('*.js'):
    gen(fn)
