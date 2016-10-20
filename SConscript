# coding: utf-8

Import('env')
env_local = env.Clone()

SConscript( 'tracing/SConscript',    exports={'env':env_local}  )
SConscript( 'sandbox/SConscript',    exports={'env':env_local}  )
