# create file.so
gcc -shared -Wall -g -c freeOnExit.c -o freeOnExit_1-0_armhf/usr/lib/freeOnExit.so -DFOE_WITH_DLL -DFOE_WITH_THREAD

# create dpkg
dpkg-deb --build freeOnExit_1-0_armhf

# file system
.
├── dpkg.md
├── freeOnExit_1-0_armhf
│   ├── DEBIAN
│   │   └── control
│   └── usr
│       ├── include
│       │   └── freeOnExit.h
│       └── lib
│           └── freeOnExit.so
├── freeOnExit.c
├── freeOnExit.h
├── init
│   ├── freeOnExit.head
│   └── freeOnExit.init
├── LICENSE
└── README.md

