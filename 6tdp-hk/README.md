# TDP6 -- Honorat & Keryell-Even

## Compilation

On your PC:
- go into *src/* folder, then execute ```make```

On plafrim
- execute command
  ```module load compiler/gcc mpi/openmpi/current```
- then go into *src/* folder and execute ```make```

## Exectution

Each binary can take one or two optional arguments:
- number_of_iterations 
- number_of_iterations grid_size

## Benchmarks

You can create perfs using
- createPerf.sh on your PC
- createPerf.pbs on Plafrim
(be aware that *6tdp-hk* must be located in your $HOME)

Then you can create graph with createGraph.sh