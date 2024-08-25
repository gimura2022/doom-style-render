project(editor)

# me_main executable
add_executable(me_main ./me_main.c)

target_link_libraries(me_main ${SDL2_LIBRARIES} ${SDL_TTF_LIBRARIES})

target_sources(me_main PRIVATE
    ./me_cmds.c
    ./me_draw.c

    ./../src/core/mem/m_alloc.c
    ./../src/core/mem/m_utils.c
    ./../src/core/sys.c
    ./../src/core/file/f_conf.c
    ./../src/game/g_map.c
    ./../src/cmd/cmd_exec.c
    ./../src/core/v_vid.c
    ./../src/core/keys/k_manager.c
    ./../src/cmd/cmd_exec.c
    ./../src/console/c_con.c
    ./../src/console/c_draw.c
    ./../src/render/r_render.c
    ./../src/utils/math/u_math.c
    ./../src/game/gfx/gfx_post_process.c
    ./../src/render/utils/r_texture.c
    ./../src/utils/math/asm/c_impl.c
    ./../src/render/draw/r_draw.c
    ./../src/lib/cbmp/cbmp.c
)

if(WIN32)
    target_link_libraries(me_main
        winmm.lib
        imm32.lib
        ole32.lib
        Setupapi.lib
        gdi32.lib
        version.lib
        Rpcrt4.lib
    )
endif()

if(UNIX)
    target_link_libraries(me_main m SDL2_ttf)
endif()
