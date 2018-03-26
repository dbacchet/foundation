# platform specific customizations
# 2013-03-28, Davide Bacchet (davide.bacchet@gmail.com)


import sys

# associate .m files to C compiler
from waflib import TaskGen
@TaskGen.extension('.m')
def m_hook(self, node):
    """Alias .m files to be compiled the same as .c files, the compiler will do the right thing."""
    return self.create_compiled_task('c', node)



def set_clang_compiler(env):
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'
    pass


def embed_MSVC_manifest(env):
    env.append_unique('LINKFLAGS', ['/MANIFEST'])
    env['WINDOWS_EMBED_MANIFEST']=True


def customize_environment(env):
    """customize the build evironment"""
    if sys.platform=='darwin':
        set_clang_compiler(env)
    elif sys.platform=='win32':
        embed_MSVC_manifest(env)
    elif sys.platform=='linux2':
        pass


