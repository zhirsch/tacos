# Create the allowed options

opts = Variables('custom.py')
opts.AddVariables(
    BoolVariable('V', 'output the commands used to build', False),
)

Return('opts')
