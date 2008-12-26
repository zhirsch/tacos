# Common definitions for all SConstruct files
import SCons

Import('opts')

# Create common build environment
env = Environment(options=opts)
env.Append(CFLAGS=['-Wall',
                   '-Werror',
                   '-Wextra',
                   '-Wno-unused-parameter',
                   '-std=c99',
                   ])

# Create a builder for mkisofs
mkisofs = SCons.Action.Action('$MKISOFSCOM', '$MKISOFSCOMSTR')
builder = Builder(action=mkisofs,
                  target_factory=env.fs.File,
                  source_factory=env.fs.Dir)
env.Append(BUILDERS={'MkIsoFs': builder})
env['MKISOFSCOM'] = 'mkisofs $MKISOFSFLAGS -o $TARGET $SOURCE'
env['MKISOFSCOMSTR'] = '$MKISOFSCOM'

# Create a builder for ln
ln = SCons.Action.Action('$LNCOM', '$LNCOMSTR')
builder = Builder(action=ln)
env.Append(BUILDERS={'HardLink': ln})
env['LNCOM'] = 'ln $SOURCE $TARGET'
env['LNCOMSTR'] = '$LNCOM'

# Set the output patterns when verbose is disabled
if not env['V']:
    env['CCCOMSTR']      = '  CC                $TARGET'
    env['LINKCOMSTR']    = '  LD                $TARGET'
    env['ASPPCOMSTR']    = '  ASPP              $TARGET'
    env['MKISOFSCOMSTR'] = '  MKISOFS           $TARGET'
    env['CPCOMSTR']      = '  COPY              $TARGET'

# Return the environment to the calling SConscript file
Return('env')
