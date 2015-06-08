import os

buildType = 'debug'

include = '#build/$BUILDTYPE/include'
lib = '#build/$BUILDTYPE/lib'
bin = '#build/$BUILDTYPE/bin'

env = Environment(BUILDTYPE = buildType,
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
                      , "-Werror"])

compiler = 'clang++'
#compiler = 'g++'
if 'CXX' in os.environ and os.environ['CXX']:
  compiler = os.environ['CXX']
env.Replace(CXX = compiler)

if compiler[:5] == 'clang':
    env.Append(CCFLAGS = "-stdlib=libc++")
    env.Append(LINKFLAGS = "-lc++")

env['PROJNAME'] = os.path.basename(Dir('.').srcnode().abspath)
print env['PROJNAME']

Export('env')
env.SConscript('src/SConscript', variant_dir='build/$BUILDTYPE')
