# Root SConstruct file

opts = SConscript('#scons/common/opts.sc')
env = SConscript('#scons/common/environ.sc', exports=['opts'])

# Enumerate the different build variants.
env.VariantDir('#bld/debug', '#src')
env.VariantDir('#bld/release', '#src')

# Build the tacos kernel for the specific variant
env.SConscript('bld/$T/SConscript', exports={'env': env.Clone()})
