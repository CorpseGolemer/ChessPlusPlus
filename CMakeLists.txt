cmake_minimum_required(VERSION 3.31)
project(ChessPlusPlus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(ChessCore STATIC
        # GAME
        src/game/Board.h
        src/game/Board.cpp
        src/game/Castling.h
        src/game/Piece.h

        # FEN
        src/fen/FenParser.h
        src/fen/FenParser.cpp

        # UTILS
        src/utils/BoardConfig.h
        src/utils/Types.h
        src/utils/Logger.h
        src/utils/Logger.cpp
)

target_include_directories(ChessCore PUBLIC src)
target_compile_features(ChessCore PUBLIC cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ChessCore PRIVATE -Wall -Wextra -Wpedantic)
endif ()

add_executable(ChessPlusPlus src/main.cpp)
target_link_libraries(ChessPlusPlus PRIVATE ChessCore)