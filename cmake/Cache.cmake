# Enable cache if available
function(fiction_enable_cache)
  set(CACHE_OPTION
      "ccache"
      CACHE STRING "Compiler cache to be used")
  set(CACHE_OPTION_VALUES "ccache" "sccache")
  set_property(CACHE CACHE_OPTION PROPERTY STRINGS ${CACHE_OPTION_VALUES})
  list(FIND CACHE_OPTION_VALUES ${CACHE_OPTION} CACHE_OPTION_INDEX)

  if(${CACHE_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
        "Using custom compiler cache system: '${CACHE_OPTION}', explicitly supported entries are ${CACHE_OPTION_VALUES}"
    )
  endif()

  # Honour the requested cache first. Searching CACHE_OPTION_VALUES directly
  # made CACHE_OPTION inert: ccache comes first in the list and is present on
  # every runner, so asking for sccache silently got ccache. find_program
  # short-circuits on a cached result, so without this a reconfigure keeps
  # whatever was found last time and CACHE_OPTION is ignored.
  unset(CACHE_BINARY CACHE)
  find_program(CACHE_BINARY NAMES ${CACHE_OPTION})
  if(NOT CACHE_BINARY)
    list(REMOVE_ITEM CACHE_OPTION_VALUES ${CACHE_OPTION})
    find_program(CACHE_BINARY NAMES ${CACHE_OPTION_VALUES})
    if(CACHE_BINARY)
      message(STATUS "${CACHE_OPTION} was not found, falling back to "
                     "${CACHE_BINARY}")
    endif()
  endif()

  if(CACHE_BINARY)
    # The Visual Studio generator ignores compiler launchers outright, so
    # configuring one there looks like it works and caches nothing.
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
      message(
        WARNING
          "${CACHE_BINARY} was found, but the '${CMAKE_GENERATOR}' generator ignores "
          "compiler launchers, so nothing will be cached. Configure with -G Ninja "
          "to make the cache effective.")
    endif()
    message(STATUS "${CACHE_BINARY} found and enabled")

    set(CACHE_LAUNCHER ${CACHE_BINARY})

    # ccache refuses to cache a compilation that uses a pre-compiled header
    # unless it is told that the PCH's macro definitions and __DATE__/__TIME__
    # expansions can be ignored. Without this, turning PCH on turns caching off,
    # which is a net loss. `env` is not available on Windows, where the two
    # options have to be combined through a ccache.conf instead.
    if(FICTION_ENABLE_PCH AND CACHE_BINARY MATCHES "ccache")
      if(WIN32)
        message(
          STATUS
            "Set `sloppiness = pch_defines,time_macros` in your ccache configuration, or ccache will not cache PCH-using compilations"
        )
      else()
        set(CACHE_LAUNCHER
            ${CMAKE_COMMAND} -E env CCACHE_SLOPPINESS=pch_defines,time_macros
            ${CACHE_BINARY})
      endif()
    endif()

    # FORCE, because without it a build tree configured before
    # FICTION_ENABLE_PCH was turned on keeps its bare `ccache` launcher and
    # never picks up the sloppiness wrapper, silently losing every PCH
    # compilation's cache entry.
    set(CMAKE_CXX_COMPILER_LAUNCHER
        ${CACHE_LAUNCHER}
        CACHE STRING "CXX compiler cache used" FORCE)
    set(CMAKE_C_COMPILER_LAUNCHER
        ${CACHE_LAUNCHER}
        CACHE STRING "C compiler cache used" FORCE)
  else()
    message(
      WARNING "${CACHE_OPTION} is enabled but was not found. Not using it")
  endif()
endfunction()
