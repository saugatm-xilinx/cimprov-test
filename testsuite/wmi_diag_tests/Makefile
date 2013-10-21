all: test_diag.exe

test_diag.exe: test_diag.cpp
	i686-w64-mingw32-g++ -Wall -W -Wno-unused -g -fms-extensions -Wno-unknown-pragmas -std=gnu++98 -static-libgcc -static-libstdc++ -Wl,--kill-at test_diag.cpp -o test_diag.exe -lole32 -loleaut32 -luuid
