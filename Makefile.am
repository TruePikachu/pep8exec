check_PROGRAMS = pep8test
pep8test_SOURCES = pep8test.cpp pep8cpu.cpp pep8mem.cpp
pep8test_LDADD = -ltap++
AM_CXXFLAGS=-DPKGDIR=\"$(top_srcdir)\"
TESTS=pep8test
noinst_HEADERS = pep8cpu.hpp pep8mem.hpp
EXTRA_DIST = pep8os.pepo tests
clean-local:
	-rm -rf *.tmp
