# -*- mode: python -*-
# Root SConstruct file

# Get the default environment
env = SConscript('#scons/common/defs.sc')

# -----------------------------------------------------------------------------

# Build the tacos kernel
exports = {
    'env': env.Clone()
}
env.SConscript('#src/SConscript', build_dir='#bld', exports=exports)
