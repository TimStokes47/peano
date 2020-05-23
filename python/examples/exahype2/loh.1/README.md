# LOH.1 in ExaHyPE2

## Tasks

* [x] Check if NCP is supported by the existing FVM solver
  * Answer: Tobias added support for NCP
* [x] Check if point sources are supported by the existing FVM solver
  * Answer: No, but not required for example.
* [x] Check if we can use a custom Riemann solver
  * Answer: We rely on the Rusanov class. This is not the optimal Riemann solver for this example but
    we do not want to implement another class (Maurizio's HLLEM Rieman solver).
    We take the higher diffusivity of this solver into account.
* [ ] Check what other fields we need:
  * gradQ: At the interfaces, this is the difference between right and left state 

##  
