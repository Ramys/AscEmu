# Copyright (c) 2014-2020 AscEmu Team <http://www.ascemu.org>

set(PATH_PREFIX GameMop/Movement)

set(SRC_GAMEMOP_MOVEMENT_FILES
    ${PATH_PREFIX}/MovementDefines.h
    ${PATH_PREFIX}/MovementStructures.h
)

source_group(GameMop\\Movement FILES ${SRC_GAMEMOP_MOVEMENT_FILES})
unset(PATH_PREFIX)
