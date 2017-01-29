# build the plugin binaries
import os
import sys
import datetime,time


basepath = str(Dir('#').abspath)

###############################################
#                ENVIRONMENT                  #
###############################################


tools = ['default']

# create the main environment
env = Environment( tools = tools )
# env = Environment( CC='clang',
#                    CXX='clang++',
#                    ENV = os.environ,
#                    tools = tools )

env.Append(BASEPATH=basepath)
env.Append(ENV = {'BASEPATH' : str(Dir('#').abspath)})


###############################################
#                   BUILD                     #
###############################################
                        
# build dir
builddir = os.path.join('build', sys.platform)
env['builddir'] = builddir
# compile flags
env.AppendUnique( CCFLAGS = ['-g',
                             # '-O2',
                             '-std=c++11',
                             '-Werror'])

# print env.Dump()
# build sources
SConscript( 'SConscript',
            variant_dir = builddir,
            duplicate=False,
            exports='env' )
