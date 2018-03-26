# define the compilation flags for the various compiler/build types
# 2007-07-02, Davide Bacchet (davide.bacchet@gmail.com)
# Last Modified, 2007-07-19, dbs

import sys

class CompilationFlags:
    "define and get the compilation flags, depending on the compiler, build mode and current environment settings"

    # initializations
    def __init__(self):
        if (sys.platform=='win32'):
            # compilation flags for c++ (msvc compiler)
            self.flags_c_common    = '/nologo /FC /openmp'
            self.flags_c_debug     = '/Z7 /W4 /Od /MDd'
            self.flags_c_release   = '/W1 /O2 /Ot /MD'
            self.flags_cpp_common  = '/EHsc /nologo /GR /FC /openmp'
            self.flags_cpp_debug   = '/Z7 /W4 /Od /MDd'
            self.flags_cpp_release = '/W1 /O2 /Ot /MD'
            self.flags_link_common = ''
            self.flags_link_debug  = '/DEBUG'
            self.flags_link_release= '/RELEASE'
            # preprocessor definitions
            self.defines_common    = 'WIN32 _MBCS'
            self.defines_debug     = '_DEBUG __Debug__ _CRT_SECURE_NO_WARNINGS'
            self.defines_release   = 'NDEBUG _CRT_SECURE_NO_WARNINGS'
        elif (sys.platform=='darwin'):
            # compilation flags for c++
            self.flags_c_common    = '-arch x86_64'
            self.flags_c_debug     = '-g'
            self.flags_c_release   = '-O2'
            self.flags_cpp_common  = '-arch x86_64'
            self.flags_cpp_debug   = '-g'
            self.flags_cpp_release = '-O2'
            self.flags_link_common = ''
            self.flags_link_debug  = ''
            self.flags_link_release= ''
            # preprocessor definitions
            self.defines_common    = 'OSX'
            self.defines_debug     = '_DEBUG __Debug__'
            self.defines_release   = 'NDEBUG'
        else:
            # compilation flags for c++
            self.flags_c_common    = '-fPIC'
            self.flags_c_debug     = '-g'
            self.flags_c_release   = '-O2'
            self.flags_cpp_common  = '-fPIC'
            self.flags_cpp_debug   = '-g'
            self.flags_cpp_release = '-O2'
            self.flags_link_common = ''
            self.flags_link_debug  = ''
            self.flags_link_release= ''
            # preprocessor definitions
            self.defines_common    = 'LINUX POSIX'
            self.defines_debug     = '_DEBUG __Debug__'
            self.defines_release   = 'NDEBUG'
                      

# create the class instance        
cf = CompilationFlags()

# get debug flags
def get_debug_defines():
    return (cf.defines_common   + ' ' + cf.defines_debug).split()
def get_c_debug_flags():
    return (cf.flags_c_common   + ' ' + cf.flags_c_debug).split()
def get_cpp_debug_flags():
    return (cf.flags_cpp_common + ' ' + cf.flags_cpp_debug).split()
def get_link_debug_flags():
    return (cf.flags_link_common + ' ' + cf.flags_link_debug).split()

# get release flags
def get_release_defines():
    return (cf.defines_common   + ' ' + cf.defines_release).split()
def get_c_release_flags():
    return (cf.flags_c_common   + ' ' + cf.flags_c_release).split()
def get_cpp_release_flags():
    return (cf.flags_cpp_common + ' ' + cf.flags_cpp_release).split()
def get_link_release_flags():
    return (cf.flags_link_common + ' ' + cf.flags_link_release).split()



