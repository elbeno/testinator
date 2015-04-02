import os

buildType = 'debug'

include = '#export/$BUILDTYPE/include'
lib = '#export/$BUILDTYPE/lib'
bin = '#export/$BUILDTYPE/bin'

env = Environment(BUILDTYPE = buildType,
                  INCDIR = include,
                  LIBDIR = lib,
                  BINDIR = bin,
                  CPPPATH = [include],
                  LIBPATH = [lib])

env.Append(CCFLAGS = "-g -std=c++1y")
env.Append(CCFLAGS = ["-pedantic"
                      , "-Wall"
                      , "-Wextra"
                      , "-Wcast-align"
                      , "-Wcast-qual"
                      , "-Wctor-dtor-privacy"
                      , "-Wdisabled-optimization"
                      , "-Wformat=2"
                      , "-Winit-self"
                      , "-Wmissing-include-dirs"
                      , "-Wold-style-cast"
                      , "-Woverloaded-virtual"
                      , "-Wredundant-decls"
                      , "-Wshadow"
                      , "-Wsign-conversion"
                      , "-Wsign-promo"
                      , "-Wstrict-overflow=5"
                      , "-Wswitch-default"
                      , "-Wundef"
                      , "-Werror"
                      , "-Wno-unused"])

compiler = 'clang++'
#compiler = 'g++'

if compiler == 'clang++':
    env.Append(CCFLAGS = "-stdlib=libc++")
    env.Append(LINKFLAGS = "-lc++")
    env.Replace(CXX = compiler)

env['PROJNAME'] = os.path.basename(Dir('.').srcnode().abspath)
print env['PROJNAME']

Export('env')
env.SConscript('src/SConscript', variant_dir='build/$BUILDTYPE')
