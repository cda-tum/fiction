
#ifndef GRAPH_COLORING_EXPORTS_H
#define GRAPH_COLORING_EXPORTS_H

#ifdef SHARED_EXPORTS_BUILT_AS_STATIC
#  define GRAPH_COLORING_EXPORTS
#  define GRAPH_COLORING_NO_EXPORT
#else
#  ifndef GRAPH_COLORING_EXPORTS
#    ifdef graph_coloring_EXPORTS
        /* We are building this library */
#      define GRAPH_COLORING_EXPORTS 
#    else
        /* We are using this library */
#      define GRAPH_COLORING_EXPORTS 
#    endif
#  endif

#  ifndef GRAPH_COLORING_NO_EXPORT
#    define GRAPH_COLORING_NO_EXPORT 
#  endif
#endif

#ifndef GRAPH_COLORING_DEPRECATED
#  define GRAPH_COLORING_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef GRAPH_COLORING_DEPRECATED_EXPORT
#  define GRAPH_COLORING_DEPRECATED_EXPORT GRAPH_COLORING_EXPORTS GRAPH_COLORING_DEPRECATED
#endif

#ifndef GRAPH_COLORING_DEPRECATED_NO_EXPORT
#  define GRAPH_COLORING_DEPRECATED_NO_EXPORT GRAPH_COLORING_NO_EXPORT GRAPH_COLORING_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GRAPH_COLORING_NO_DEPRECATED
#    define GRAPH_COLORING_NO_DEPRECATED
#  endif
#endif

#endif /* GRAPH_COLORING_EXPORTS_H */
