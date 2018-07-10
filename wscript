#! /usr/bin/env python
# encoding: utf-8

import os
import sys


# the following two variables are used by the target "waf dist"
VERSION = '0.0.1'
APPNAME = 'bfengine'


top = '.'
out = 'build'

basepath = os.path.abspath(top)
sys.path.append(os.path.join(basepath, 'var', 'waf'))

# platform specific settings/utilies
import platformcustom



# waf function cal order:
#   options()
#   init()
#   configure()
#   build()


def options(opt):
    opt.load('compiler_c compiler_cxx waf_unit_test')
    # overwrite default install prefix
    opt.add_option('--prefix', action='store', default='bf_install', help='final install dir')

def init(ctx):
    # setup variant builds (debug/release)
    from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context', '').lower()
        class TmpDebugCtx(y):
            cmd = name + '_debug'   # command names like 'build_debug', 'clean_debug', etc
            variant = 'debug'
        class TmpReleaseCtx(y):
            # if a 'cmd' is not given, the default 'build', 'clean', etc actions will be redirected to the 'release' variant dir
            variant = 'release'


def configure(conf):
    # apply platform specific configurations
    platformcustom.customize_environment(conf.env)
    # load compiler tools
    conf.load('compiler_c compiler_cxx waf_unit_test')

    # apply debug variant configurations
    # base_env = conf.env.derive()

    import compileroptions
    conf.setenv('debug', env=conf.env.derive().detach())
    conf.env.BUILD_MODE = 'debug'
    conf.env.append_unique('DEFINES',   compileroptions.get_debug_defines())
    conf.env.append_unique('CFLAGS',    compileroptions.get_c_debug_flags())
    conf.env.append_unique('CXXFLAGS',  compileroptions.get_cpp_debug_flags())
    conf.env.append_unique('LINKFLAGS', compileroptions.get_link_debug_flags())
    conf.recurse('3rdparty')
    # apply release variant configurations
    conf.setenv('release', env=conf.env.derive().detach())
    conf.env.BUILD_MODE = 'release'
    conf.env.append_unique('DEFINES',   compileroptions.get_release_defines())
    conf.env.append_unique('CFLAGS',    compileroptions.get_c_release_flags())
    conf.env.append_unique('CXXFLAGS',  compileroptions.get_cpp_release_flags())
    conf.env.append_unique('LINKFLAGS', compileroptions.get_link_release_flags())
    conf.recurse('3rdparty',once=False) # once=False is needed, otherwise this 'recurse' call will not be execute (because already called for debug variant)


def build(bld):
    # add the base dir to the environment
    bld.env.BASEDIR = basepath
    # go into subdirs
    bld.recurse('3rdparty')
    bld.recurse('containers')
    bld.recurse('logger')
    bld.recurse('src')

