# Root SConstruct file

opts = SConscript('#scons/common/opts.sc')
env = SConscript('#scons/common/environ.sc', exports=['opts'])

# Build the tacos kernel
env.VariantDir('#bld', '#src')
exports = {
    'env': env.Clone(),
}
env.SConscript('#bld/SConscript', exports=exports)
