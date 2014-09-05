# -*- mode: python -*-
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
mkisofs_builder = Builder(action=Action('$MKISOFSCOM', '$MKISOFSCOMSTR'),
                          source_factory=env.fs.Dir)
env.Append(BUILDERS={'MkIsoFs': mkisofs_builder})
env['MKISOFSCOM'] = 'mkisofs $MKISOFSFLAGS -o $TARGET $SOURCE'
env['MKISOFSCOMSTR'] = '$MKISOFSCOM'

# Create a builder for sed
env.Append(BUILDERS={'Sed': Builder(action=Action('$SEDCOM', '$SEDCOMSTR'))})
env['SEDCOM'] = 'sed -e"$PATTERN" < $SOURCE > $TARGET'
env['SEDCOMSTR'] = '$SEDCOM'

# Set the output patterns when verbose is disabled
if not env['V']:
    env['CCCOMSTR']      = '  CC                $TARGET'
    env['LINKCOMSTR']    = '  LD                $TARGET'
    env['ASPPCOMSTR']    = '  ASPP              $TARGET'
    env['MKISOFSCOMSTR'] = '  MKISOFS           $TARGET'
    env['SEDCOMSTR']     = '  SED               $TARGET'
    env['COPYSTR']       = '  COPY              $TARGET'
    SCons.Script.Copy.strfunc = lambda d, s: env.subst('$COPYSTR', 1, target=d)

# Return the environment to the calling SConscript file
Return('env')
