

add_library(l_net STATIC)

if(WIN32)
    target_sources(l_net PRIVATE
                l_net_wins.c
                l_net_wins.h
                l_net.c
                l_net.h
    )

    target_link_libraries(l_net PRIVATE wsock32 ws2_32 ole32)
else()
    target_sources(l_net PRIVATE
                l_net_berkley.c
                l_net.c
                l_net.h
    )
endif()