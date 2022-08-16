# 02.08.2022
# Coskun ERGAN

cmake_minimum_required(VERSION 3.15)

#### setable variables ####
set(SDCC_EXECUTABLE "C:/SDCC/bin/sdcc.exe" CACHE PATH "path to sdcc executable" FORCE)

############# toolchain info, must come before project definition ##############
set(CMAKE_SYSTEM_PROCESSOR BF7615BM44-LJTX)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${SDCC_EXECUTABLE})

############################## Version Forward Info #############################

file(READ VERSION.txt VERSION_STR)

# Obtain number
STRING(REGEX MATCHALL "[0-9]" VERSION_PARTS "${VERSION_STR}")
LIST(GET VERSION_PARTS 0 VERSION_MAJOR)
LIST(GET VERSION_PARTS 1 VERSION_MINOR)
LIST(GET VERSION_PARTS 2 VERSION_PATCH)

add_compile_definitions(VERSION_MAJOR=${VERSION_MAJOR})
add_compile_definitions(VERSION_MINOR=${VERSION_MINOR})
add_compile_definitions(VERSION_PATCH=${VERSION_PATCH})

# preprocessor and linker flags
set(CMAKE_C_FLAGS_INIT "--model-large -I. -I../Common/include -I../include -I../../Source/include --less-pedantic --xram-size 4352 --stack-auto --no-peep --int-long-reent --float-reent")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--model-large -I. -I../Common/include -I../include -I../../Source/include --less-pedantic --xram-size 4352 --stack-auto --no-peep --int-long-reent --float-reent")

############################## Project Definition ##############################

project("FREERTOS_8051_TEMP" C) 

set(CMAKE_BUILD_TYPE "Debug")

set(COMPILER_CLASSB_FLASH_END_ADDRES 0xFBF0) #64k

add_definitions(
    -DCOMPILER_CLASSB_FLASH_END_ADDRES=${COMPILER_CLASSB_FLASH_END_ADDRES}
)

file(GLOB PROJECT_SOURCES
	"Demo/Byd/main.c"	    
    "Demo/Byd/port.c"
    "Source/portable/MemMang/heap_1.c"	
	"Source/tasks.c"
	"Source/queue.c"
	"Source/list.c"
	"Demo/Common/Minimal/flash.c"
	"Demo/Common/Full/print.c"     
	"Demo/Common/Minimal/integer.c"
	"Demo/Common/Minimal/PollQ.c"
	"Demo/Common/Minimal/comtest.c"
    "Demo/Common/Minimal/i2ctest.c"
	"Demo/Common/Full/semtest.c" 
	"Demo/Byd/ParTest/ParTest.c"
	"Demo/Byd/serial/serial.c"           
    "Demo/Byd/i2c/i2c_slave.c" 
)

include_directories(
    #"C:/SDCC/include"
    #"C:/SDCC/include/mcs51"
	"Demo/Byd"
	"Demo/Common/include"
	"Source/include"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

############################## Output Procces ##############################

# generate CRC in hex file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT
        "Generate CRC16 in the .hex file"
        DEPENDS
        ${PROJECT_NAME}.ihx        
    COMMAND
    ../Tools/srec_cat.exe ${PROJECT_NAME}.ihx -intel -crop 0 ${COMPILER_CLASSB_FLASH_END_ADDRES} -fill 0xFF 0x0000 ${COMPILER_CLASSB_FLASH_END_ADDRES} -crc16-l-e ${COMPILER_CLASSB_FLASH_END_ADDRES} -o ../Output/Project.hex -intel -address-length=2
)

# generate hex to bin file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT
        "Generate bin on the .hex file"
    DEPENDS
    ../Output/Project.hex
    COMMAND
    ../Tools/srec_cat.exe ../Output/Project.hex -Intel -o ../Output/BinaryFile.bin -Binary
)

############################## Console Information ##############################

# print buil info
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo " Build Information                                "
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo " PROJECT_NAME            : ${PROJECT_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo " PROJECT_VERSION         : ${VERSION_STR}"
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo " CMAKE_SYSTEM_PROCESSOR  : ${CMAKE_SYSTEM_PROCESSOR}"
    COMMAND ${CMAKE_COMMAND} -E echo " CMAKE_AR                : ${CMAKE_AR}"
    COMMAND ${CMAKE_COMMAND} -E echo " CMAKE_C_COMPILER        : ${CMAKE_C_COMPILER}"
    COMMAND ${CMAKE_COMMAND} -E echo " CMAKE_VERSION           : ${CMAKE_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
)
