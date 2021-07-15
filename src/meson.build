simRed_sources = [
    'SimRedGGkk.cc',
    'Enlace.cc',
    'Trafico.cc',
    'BatchesDrved.cc',
    'Batches.cc',
    'DpdentSmplStat.cc',
    'MRG.cc',
    'Constant.cc',
    'Erlang.cc',
    'GetOpt.cc',
    'LogNorm.cc',
    'NegExp.cc',
    'Normal.cc',
    'SmplStat.cc',
    'Uniform.cc',
    'Weibull.cc',
    'SmplStatDrved.cc',
    'Comunicacion.cc'
]

MM_sources = [
    'SimRedGGkk.cc',
    'Enlace.cc',
    'Trafico.cc',
    'BatchesDrved.cc',
    'Batches.cc',
    'DpdentSmplStat.cc',
    'MRG.cc',
    'GetOpt.cc',
    'SmplStat.cc',
    'SmplStatDrved.cc',
    'Random.cc',
    'Uniform.cc',
    'Comunicacion.cc'
]

executable('SimRedG1kk', simRed_sources, cpp_args: '-DSIMMODE=G1')
executable('SimRedGDkk', simRed_sources, cpp_args: '-DSIMMODE=GD')
executable('SimRedGGkk', simRed_sources, cpp_args: '-DSIMMODE=GG')
executable('SimRedM1kk', simRed_sources, cpp_args: '-DSIMMODE=M1')
executable('SimRedMMkk', simRed_sources, cpp_args: '-DSIMMODE=MM')
executable('generaKrupp', 'generaKrupp.cc', 'GetOpt.cc')
