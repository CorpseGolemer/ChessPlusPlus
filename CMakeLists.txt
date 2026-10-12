cmake_minimum_required(VERSION 3.31)
project(ChessPlusPlus)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(ChessPlusPlus Sources/main.cpp
        Sources/Pieces/Piece.cpp
        Headers/Pieces/Piece.h
        Headers/Game/Board.h
        Sources/Game/Board.cpp
        Sources/Pieces/Rook.cpp
        Headers/Pieces/Rook.h
        Headers/App.h
        Sources/App.cpp
        Headers/Game/Game.h
        Sources/Game/Game.cpp
        Headers/Game/Move.h
        Sources/Game/Move.cpp
        Headers/Pieces/Bishop.h
        Sources/Pieces/Bishop.cpp
        Headers/Pieces/Queen.h
        Sources/Pieces/Queen.cpp
        Headers/Pieces/Knight.h
        Sources/Pieces/Knight.cpp
        Headers/Pieces/Pawn.h
        Sources/Pieces/Pawn.cpp
        Headers/Pieces/King.h
        Sources/Pieces/King.cpp
        Headers/Game/GameManager.h
        Sources/Game/GameManager.cpp
        Headers/Graphics/BoardRenderer.h
        Sources/Graphics/BoardRenderer.cpp
        Headers/Graphics/Colors.h
        Sources/Graphics/Colors.cpp
        Headers/Graphics/Resources.h
        Headers/Graphics/PromotionRenderer.h
        Sources/Graphics/PromotionRenderer.cpp
        Headers/Game/Zobrist.h
        Sources/Game/Zobrist.cpp
        Sources/Game/GameHistory.cpp
        Headers/Game/GameHistory.h
        Headers/Game/IGameObserver.h
        Headers/Graphics/UI/Button.h
        Sources/Graphics/UI/Button.cpp
        Sources/Graphics/Resources.cpp
        Headers/Graphics/UI/UIManager.h
        Sources/Graphics/UI/UIManager.cpp
        Headers/Graphics/UI/GameStateUIRenderer.h
        Sources/Graphics/UI/GameStateUIRenderer.cpp
        Headers/Graphics/UI/ButtonManager.h
        Sources/Graphics/UI/ButtonManager.cpp
        Headers/AppContext.h
        Headers/Bots/IBot.h
        Headers/Bots/ChciwyJakub.h
        Sources/Bots/ChciwyJakub.cpp
        Headers/Bots/Golemiarz.h
        Sources/Bots/Golemiarz.cpp
        Headers/Utils/GameManagerUtils.h
        Sources/Utils/GameManagerUtils.cpp
)

target_compile_features(ChessPlusPlus PRIVATE cxx_std_17)
target_link_libraries(ChessPlusPlus PRIVATE SFML::Graphics)