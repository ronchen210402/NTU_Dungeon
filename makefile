TARGET = NTUDungeon

CC = g++
CCFLAGS = -g -Wall -std=c++11 -lSDL2 -lSDL2_image -lSDL2_ttf -Wno-unused-command-line-argument

SRC_DIR = src

IMG_DIR  = ${SRC_DIR}/img
OBS_DIR  = ${SRC_DIR}/observe
SAV_DIR  = ${SRC_DIR}/sav
MAIN_DIR = ${SRC_DIR}/main

INC_DIR = -I${IMG_DIR} \
          -I${OBS_DIR} \
          -I${SAV_DIR} \
          -I${MAIN_DIR}\
          -Iinclude

SRC  = ${wildcard  ${IMG_DIR}/*.cpp} \
       ${wildcard  ${OBS_DIR}/*.cpp} \
       ${wildcard  ${SAV_DIR}/*.cpp} 
OBJ  = ${patsubst %.cpp, %.o, ${SRC}}

MAIN = ${wildcard  ${MAIN_DIR}/*.cpp}

${TARGET}: ${MAIN} ${OBJ}
	@echo "Linking $< ..."
	@${CC} $^ ${CCFLAGS} ${INC_DIR} -o $@
	@echo "Compile done."

$(OBJ):%.o:%.cpp %.h
	@echo "Compiling $< ==> $@"
	@${CC} ${CCFLAGS} ${INC_DIR} -c $< -o $@

clean:
	@rm -f ${OBJ}
	@echo "Clean object files done."

	@rm -f *~
	@echo "Clean tempreator files done."

	@echo "Clean done."
