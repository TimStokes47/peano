# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations


class PDETerm:
    """!

    Represents one named, point-wise PDE operator that a solver exposes for
    the user to implement: a C++ method name, its parameter list, and an
    implementation. flux(), eigenvalues(), the nonconservative product,
    algebraic/point sources, boundary conditions, ... all have this same
    shape (see python/exahype2/solvers/fv/riemann/kernels.py for how they
    are actually declared in the generated abstract solver header).

    Well-known terms should get their own PDETerm subclass which pins down
    name and signature and adds any term-specific behaviour - @see Flux for
    a fully worked example. Use the plain PDETerm constructor directly only
    for a new, solver-specific term that does not warrant its own subclass
    (yet).

    A term's implementation is either one of the three sentinel strings
    below - PDETerm.USER_DEFINED (the generated C++ method is left pure
    virtual for the user to implement by hand), PDETerm.NONE (the term is not
    generated at all, i.e. the PDE does not have this contribution), or
    PDETerm.EMPTY (the method is generated but does nothing) - or an actual
    C++ code snippet that becomes the method body verbatim. These correspond
    1:1 to python/exahype2/solvers/PDETerms.py's User_Defined_Implementation/
    None_Implementation/Empty_Implementation; they are redeclared here so
    that exahype2.api does not have to reach back into the legacy
    exahype2.solvers tree for three string constants.

    PDETerm only models *what* the term is (name, signature, implementation).
    It deliberately knows nothing about Jinja2 or the code generator: turning
    a solver's collection of terms into the generated abstract solver header
    remains the code generator's job, exactly as it is today.

    @see exahype2.api.Solver.add_pde_term()

    """

    USER_DEFINED = "<user-defined>"
    NONE = "<none>"
    EMPTY = "<empty>"

    def __init__(self,
                 name: str,
                 implementation: str  = None,
                 stateless:      bool = False,
                 ):
        """!

        name: str
          The C++ method name, e.g. "flux" or "eigenvalues". Used by
          exahype2.api.Solver to key its collection of terms, i.e. has to be
          unique per solver.

        implementation: str
          One of PDETerm.USER_DEFINED/NONE/EMPTY, or a C++ snippet that
          becomes the method body. None (the default) is treated the same as
          PDETerm.NONE, i.e. the term is switched off until the user
          actively enables it.

        """
        self.name           = name
        self.implementation = implementation
        self.stateless      = stateless

    @property
    def is_user_defined(self) -> bool:
        """! Method is generated as pure virtual for the user to implement. """
        return self.implementation == PDETerm.USER_DEFINED

    @property
    def is_disabled(self) -> bool:
        """! Term is switched off - the method is not generated at all. """
        return self.implementation is None or self.implementation == PDETerm.NONE

    @property
    def is_empty(self) -> bool:
        """! Method is generated but its body does nothing. """
        return self.implementation == PDETerm.EMPTY

    @property
    def has_custom_implementation(self) -> bool:
        """! Implementation is an actual C++ snippet rather than one of the three sentinels. """
        return not (self.is_user_defined or self.is_disabled or self.is_empty)

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}(name={self.name!r}, implementation={self.implementation!r})"

    def __str__(self) -> str:
        return f"{self.name}: {self.implementation}"

    class Generator:
        """!

        Turns a PDETerm into the C++ text for its generated method, following
        the pattern used throughout exahype2's Finite Volume kernels (see
        python/exahype2/solvers/fv/riemann/kernels.py): a disabled term is
        not declared at all; a user-defined term is declared pure virtual for
        the user to implement by hand; anything else is declared as a
        generated, non-overridable ("final") method whose body becomes the
        definition. A stateless term additionally gets a GPU-offloadable
        static overload declared alongside the virtual one.

        Embedded into PDETerm (reachable as term.generator) so that a term
        subclass can supply its own Generator - @see Flux.Generator - and
        term.generator resolves to the right one automatically, since it is
        looked up on the term's actual class rather than hardcoded to
        PDETerm.Generator.

        """

        def __init__(self, term: "PDETerm"):
            self.term = term

        @property
        def signature(self) -> str:
            """!
            The term's C++ parameter list. Looked up as a SIGNATURE attribute
            on the owning term (e.g. Flux.SIGNATURE); a plain, custom PDETerm
            has to set term.SIGNATURE itself before it can generate code.
            """
            signature = getattr(self.term, "SIGNATURE", None)
            if signature is None:
                raise AttributeError(
                    f"{self.term!r} (or its class) has no SIGNATURE to generate code from; "
                    "either subclass PDETerm and provide one (see Flux), or set "
                    "term.SIGNATURE explicitly."
                )
            return signature

        def _generate_static_offloadable_declaration(self) -> str:
            if not self.term.stateless or self.term.is_user_defined:
                return ""
            return (
                "#if defined(GPUOffloadingOMP)\n"
                "#pragma omp declare target\n"
                "#endif\n"
                f"static GPUCallableMethod void {self.term.name}(\n"
                f"  {self.signature},\n"
                "  Offloadable\n"
                ");\n"
                "#if defined(GPUOffloadingOMP)\n"
                "#pragma omp end declare target\n"
                "#endif\n"
            )

        def generate_declaration(self) -> str:
            """!
            Declaration text for the abstract solver header: the (optional)
            GPU-offloadable static overload followed by the virtual method,
            or the empty string if the term is disabled.
            """
            if self.term.is_disabled:
                return ""
            body_suffix = " = 0" if self.term.is_user_defined else " final"
            return (
                self._generate_static_offloadable_declaration()
                + f"virtual void {self.term.name}(\n"
                + f"  {self.signature}\n"
                + f"){body_suffix};\n"
            )

        def generate_definition(self) -> str:
            """!
            Body of the generated method, or the empty string if the term is
            disabled or the user implements it by hand (nothing to generate
            in either case).
            """
            return self.term.implementation if self.term.has_custom_implementation else ""

    @property
    def generator(self) -> "PDETerm.Generator":
        """!
        Code generator for this term: an instance of this term's own
        (possibly subclass-specific) Generator. @see PDETerm.Generator
        """
        return self.__class__.Generator(self)

