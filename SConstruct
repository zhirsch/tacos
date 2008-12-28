# Root SConstruct file

opts = SConscript('#scons/common/opts.sc')
env = SConscript('#scons/common/environ.sc', exports=['opts'])

# Enumerate the different build variants.
env.VariantDir('#bld/debug', '#src')
env.VariantDir('#bld/release', '#src')

# Build the tacos kernel for each specified variant.
for variant in env['T']:
    env['T'] = variant
    env.SConscript('bld/%s/SConscript' % variant, exports=['env'])
