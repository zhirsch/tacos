# Create the allowed options

opts = Variables('custom.py')
opts.AddVariables(
    BoolVariable('V', 'output the commands used to build', False),
    ('O', 'the optimization level to use', 0),
)

Return('opts')
