#! /bin/sh

abort() {
  echo "$@"
  exit 1
}

target="$1"

echo -n "${target}.cc,"


if grep -sq ${target} ignore.txt ; then
  echo
  exit 0
fi

make "${target}.xcodeml" >&2 || abort 'CXXtoXcodeML failed'
make "${target}.dst.cpp" >&2 || abort 'XcodeMLtoCXX failed'

echo -n 'OK,' # CXX -> XML conversion was correctly done

clang++ -o "${target}.out" "${target}.dst.cpp" || abort 'Compilation failed'

echo -n 'OK, ' # XML -> C++ conversion was correctly done

echo $(./"${target}.out")
