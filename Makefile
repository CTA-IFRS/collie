
CC = clang++
FLAGS = -ObjC++ -std=c++17 -Wall -O3  -pedantic-errors \
		-I${OPENCV_PATH}/include/opencv4 \
		$(shell fltk-config --cxxflags) \
		-Isrc
FLAGS_LIBS = -framework CoreGraphics -framework CoreFoundation -framework Cocoa \
			 -lopencv_core -lopencv_imgproc  -lopencv_imgcodecs $(shell fltk-config --ldflags) \
			 -L${OPENCV_PATH}/lib -Wl,-rpath,'@executable_path'
SRC_CPP = $(shell find src -name "*.cpp")
SRC_FLUID = $(shell find src -name "*.cxx")
SRC_MM = $(shell find src -name "*.mm")
SRC = ${SRC_CPP} ${SRC_MM} ${SRC_FLUID}
OBJS = $(subst .cxx,.o,$(subst .mm,.o,$(subst src/,build/src/,$(subst .cpp,.o,${SRC}))))
APP = collie

.PHONY: all clean run

all: ${APP}

${APP}: ${OBJS}
	${CC} $^ ${FLAGS_LIBS} -o ${APP}

build/%.o: %.cpp 
	if [ ! -d "$(shell dirname $@)" ]; then mkdir -p "$(shell dirname $@)"; fi
	${CC} $< -c ${FLAGS} -o $@

build/%.o: %.cxx 
	if [ ! -d "$(shell dirname $@)" ]; then mkdir -p "$(shell dirname $@)"; fi
	${CC} $< -c ${FLAGS} -o $@

build/%.o: %.mm 
	if [ ! -d "$(shell dirname $@)" ]; then mkdir -p "$(shell dirname $@)"; fi
	${CC} $< -c ${FLAGS} -o $@

clean:
	if [ -d build ]; then rm -rf build; fi
	if [ -f ${APP}  ]; then rm ${APP}; fi
	if [ -d "${APP}.app" ]; then rm -rf "${APP}.app"; fi

run: all
	LD_LIBRARY_PATH=${OPENCV_PATH}/lib ./${APP}

app: all
	mkdir -p "${APP}.app"/Contents/MacOS
	cp resources/info.plist "${APP}.app"/Contents
	cp ${APP} "${APP}.app"/Contents/MacOS
	cp ${OPENCV_PATH}/lib/libopencv_core.407.dylib "${APP}.app"/Contents/MacOS
	cp ${OPENCV_PATH}/lib/libopencv_imgproc.407.dylib "${APP}.app"/Contents/MacOS
	cp ${OPENCV_PATH}/lib/libopencv_imgcodecs.407.dylib "${APP}.app"/Contents/MacOS
