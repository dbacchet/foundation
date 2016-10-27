# coding: utf-8

Import('env')
env_local = env.Clone()

SConscript( '3rdparty/SConscript',   exports={'env':env_local}  )
SConscript( 'common/SConscript',     exports={'env':env_local}  )
SConscript( 'tracing/SConscript',    exports={'env':env_local}  )
SConscript( 'testing/SConscript',    exports={'env':env_local}  )
