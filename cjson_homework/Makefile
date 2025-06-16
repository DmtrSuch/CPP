# PY_incl=$(shell python3-config --includes)
# PY_exten=$(shell python3-config --extension-suffix)
# PY_ldflags=$(shell python3-config --ldflags)
MODULE_NAME = cjson

.PHONY: all build install clean uninstall

all: build

# build:
# 	g++ -O2 -Wall -shared -std=c++20 -fPIC $(PY_incl) -o cjson$(PY_exten) cjson.c $(PY_ldflags)

build:
	python3 setup.py build

install:
	python3 setup.py install --user

test:
	./test.py

clean:
	rm -rf build/ dist/ *.egg-info __pycache__ *.so

uninstall:
	@echo "Deletling $(MODULE_NAME)..."
	@pip uninstall -y $(MODULE_NAME)

# clean:
#	rm -f *.so
