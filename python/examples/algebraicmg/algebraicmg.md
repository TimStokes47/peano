# Algebraic multigrid solvers # 

A collection of simple multigrid solvers which demonstrate, step by step, how 
one can write a multigrid solver within Peano. Each individual Python script 
in the root directory here is one complete solver example for the same 
pathologic setup. The simplest one is the Jacobi solver with a geometric, 
low-order rediscretisation. From thereon, each example adds a different level 
of complexity. Some of them build upon each other. Others are orthogonal.



## Solver variants ##

- geometric-jacobi
    Baseline code that we use as a template for all of the subsequent examples.
    It solves the problem with a d-linear elements where the material parameter
    is sampled once per cell.
    
    Files that have to be studied/defined for this setup:
    - SetupScenario - generic setup class holding the problem description that 
      I use for all of the subsequent examples.
    - JacobiUpdate - this is a generic update of the Jacobi smoother
    - ComputeGlobalResidual - again a generic action set. As we know the 
      analytic solution, we can compute  the global residuals, too.
    
    
## todos ##

- Compute global residual should only evaluate fine grid vertices. For this, I finally need
  the vertex markers
- Parallelise geometric variant
- Add AMR
- Add more complex stencils


