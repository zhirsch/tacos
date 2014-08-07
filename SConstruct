# -*- mode: python -*-
# Root SConstruct file

opts = SConscript('#scons/common/opts.sc')
base_env = SConscript('#scons/common/environ.sc', exports=['opts'])

# Enumerate the different build variants.
base_env.VariantDir('#bld/debug', '#')
base_env.VariantDir('#bld/release', '#')

# Build the tacos kernel for each specified variant.
for variant in base_env['T']:
    env     = base_env.Clone(T=variant)
    kernel = env.SConscript('bld/%s/src/SConscript' % variant, exports=['env'])
    iso    = env.SConscript('bld/%s/iso/SConscript' % variant, exports=['env', 'kernel'])
    bochs  = env.SConscript('bld/%s/bochs/SConscript' % variant, exports=['env', 'iso'])
    env.Default([kernel, iso, bochs])
