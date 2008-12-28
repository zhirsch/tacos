# Create the allowed options

opts = Variables('custom.py')
opts.AddVariables(
    BoolVariable('V', 'echo the build commands', False),
    EnumVariable('T', 'the build variant type', 'debug', ['debug', 'release']),
)

Return('opts')
