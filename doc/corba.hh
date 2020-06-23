/**
\page hpp_corbaserver_bind_cpp_objects Bind C++ objects automatically

\par Module hierarchy.
It should respect the C++ hierarchy. At the moment, the topmost
module is \c hpp. The other module should either be as in C++ or suffixed with \c _idl.
Suffixing is usefull to prevent name collisions.
For instance, let us consider idl file in \c idl/mod1/mod2_idl/class1.idl containing
\code{.idl}
module mod1 {
  module mod2_idl {
    Class1  {
      void method1() raises(hpp::Error);
      //-> method2
      void method3() raises(hpp::Error);
      //* some code
      //* some code
  };
}
\endcode

\par IDL comments prefix
They are used to trigger particular behavior.
- '// ' is a standard comment (two slashes followed by one space).
- '//*' replaces the automatic implementation by the code provided. All
        consecutive lines starting with this prefix will be used.

        You also find them after the hpp module. The code in this context will
        be put at the beginning of the generated C++ file. This is useful to
        include C++ headers.
- '//->' is used to rename a function, a module or an interface (the last may not be implemented yet).
- '///' is a comment used to generate the documentation (by Python and Doxygen).

\par Generation of the code with omniidl

To generate the python and C++ code, use the following cmake macros:
\code
GENERATE_IDL_CPP (mod1/mod2_idl/${IDL} ${CMAKE_SOURCE_DIR}/idl/mod1/mod2_idl
  HEADER_SUFFIX -idl.hh)
GENERATE_IDL_PYTHON (${IDL} ${CMAKE_SOURCE_DIR}/idl/mod1/mod2_idl
  ENABLE_DOCSTRING
  STUBS mod1_stubs.mod2
  ARGUMENTS
  -Wbmodules=mod1_idl
  -Wbextern=common:hpp_stubs
  -Wbextern=constraints:hpp_stubs.constraints
  -Wbextern=robots:hpp_stubs.pinocchio
  -Wbextern=paths:hpp_stubs.core
  -Wbextern=_constraints:hpp_stubs.core
  )
GENERATE_IDL_CPP_IMPL (mod1/mod2_idl/discretization ${CMAKE_SOURCE_DIR}/idl/mod1/mod2_idl
  ARGUMENTS
  -Wbguard_prefix=mod1_mod2_idl
  -Wbinc_prefix=mod1/mod2_idl
  HH_SUFFIX -idl.hh)
\endcode
where
\li \c STUBS mod1_stubs.mod2 is the python module where the stubs (...) are generated,
\li \c -Wbmodules ...,
\li \c -Wextern=common:hpp_stubs means that symbols defined in class1
        \c common.idl can be found in python module hpp_stubs,

the following C++ files are generated:
\li \c discretization-fwd.hh containing
\code
mod1::corbaServer::mod2_impl::class1Servant <_Base, _Storage>
  public ServantBase<mod1::mod2::class1, _Storage>,
  public virtual _Base
\endcode
\li \c class1.hh
\code
mod1::corbaServer::mod2_impl::class1Servant <_Base, _Storage>
\endcode
\li \c class1-idl.hh
\code
mod1::mod2_idl::class1
\endcode

The following python modules are generated
\li mod1

\par What to do when the automatic implementation does not work ?

1. The compiler should tell you what function fails.
2. Understand why this function fails. You can modify the file in the build folder.
3. When your changes work as expected, copy the code in a *comment block for code*
   (i.e. comments starts with //*) just after the function declaration.
   No need to include the try/catch block.

As an example, take function DifferentiableFunction.value, in
idl/hpp/constraints_idl/constraints.idl.
The automatic implementation for this function cannot work since LieGroupElement
cannot be converted (at least, as of now). So you must provide a implementation.
Have a look at the comment in the file.

\par Use CMake to generate the C++ implementation of your IDL.
The best is to have a look at src/CMakeLists.txt. Eventually,
hpp-manipulation-corba/src/CMakeLists.txt may be informative.
Most of the complexity is hidden behind the CMake macros
\c GENERATE_IDL_PYTHON, \c GENERATE_IDL_CPP and \c GENERATE_IDL_CPP_IMPL.

If you encounter an error related to a module not found, it may be because you
forgot an option *-Wbextern=...* in the CMake function GENERATE_IDL_PYTHON

\todo The fact that one must specify what are the external module, for the Python
client, is very annoying. One workaround could be to generate only one big Python
file instead of plenty of separated ones. To do this, it should be sufficient to
pass multiple IDL files to omniidl.
*/
