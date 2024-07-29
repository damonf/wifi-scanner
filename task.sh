#!/bin/bash -e

declare -r script="${0#./}"
declare -r use_ninja=1
declare -r use_clang=0
declare -r use_clang_stdlib=0

declare -a clang_compiler=("-DCMAKE_C_COMPILER=clang" "-DCMAKE_CXX_COMPILER=clang++")
declare -a clang_stdlib=("-DCMAKE_CXX_FLAGS=-stdlib=libc++" "-DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ -lc++abi")

usage()
{
  local b n

  b=$(tput bold)
  n=$(tput sgr0)

  printf "%sbuild%s\tbuild the project (default)\n" "$b" "$n"
  printf "%s [-r] [-d] [build]\n" "$script"
  printf "\t-r = rebuild\n"
  printf "\t-d = rebuild dependencies\n"
  printf "\n"

  printf "%stest%s\trun tests\n" "$b" "$n"
  printf "%s [-f] test\n" "$script"
  printf "\t-f = run failed tests only\n"
  printf "\n"
  
  printf "%stidy%s\trun clang-tidy\n" "$b" "$n"
  printf "%s [-f] tidy\n" "$script"
  printf "\t-f = -fix (apply suggested fixes)\n"
  printf "\n"
  
  printf "%sformat%s\trun clang-format\n" "$b" "$n"
  printf "%s format\n" "$script"
  printf "\n"

  printf "%sclean%s\tclean the project\n" "$b" "$n"
  printf "%s [-d] clean\n" "$script"
  printf "\t-d = also clean dependencies\n"
}

clean()
{
  [ -f compile_commands.json ] && rm compile_commands.json 
  rm -rf build
}

clean_deps()
{
  rm -rf third_party/bin
  rm -rf third_party/build
}

build_catch2()
{
  if [ ! -d third_party/bin/Catch2 ]; then
    pushd third_party/Catch2

    local -a args=("-DBUILD_TESTING=OFF" "-DCMAKE_INSTALL_PREFIX=../bin/Catch2")

    if [ "$use_clang" -eq 1 ]; then
      args+=("${clang_compiler[@]}")
    fi

    if [ "$use_clang_stdlib" -eq 1 ]; then
      args+=("${clang_stdlib[@]}")
    fi

    cmake -B ../build/Catch2 -S . "${args[@]}"
    cmake --build ../build/Catch2/ --target install

    popd
  fi
}

build_configure()
{
  pushd build

  local -a args=("-DCMAKE_BUILD_TYPE=Debug")
  
  if [ "$use_ninja" -eq 1 ]; then
    args+=("-G Ninja")
  fi

  if [ "$use_clang" -eq 1 ]; then
    args+=("${clang_compiler[@]}")
  fi

  if [ "$use_clang_stdlib" -eq 1 ]; then
    args+=("${clang_stdlib[@]}")
  fi

  cmake .. "${args[@]}" 

  popd
}

build_project()
{
  if [ ! -d build ]; then
    mkdir build
    build_configure
  fi

  pushd build
  cmake --build .
  popd

  if [ ! -e compile_commands.json ]; then
    ln -s build/compile_commands.json compile_commands.json
  fi
}

build()
{
  build_catch2
  build_project
}

build_task()
{
  local r d

  while getopts ":rd" opt; do
  
    case $opt in 
  
      r ) r=1 ;;
  
      d ) d=1 ;;
  
      * )
        usage
        exit 1
  
    esac
  
  done
  
  shift $((OPTIND - 1))

  if [[ -v $1 && $1 != "build" ]]; then
    usage
    exit 1
  fi

  if (( r == 1 )); then
    clean
  fi
  if (( d == 1 )); then
    clean_deps
  fi

  build
}

clean_task()
{
  local d

  while getopts ":d" opt; do
  
    case $opt in 
  
      d ) d=1 ;;
  
      * )
        usage
        exit 1
  
    esac
  
  done
  
  shift $((OPTIND - 1))

  if [[ -v $1 && $1 != "clean" ]]; then
    usage
    exit 1
  fi

  if (( d == 1 )); then
    clean_deps
  fi

  clean
}

test_task()
{
  local f args

  while getopts ":f" opt; do
  
    case $opt in 
  
      f ) f=1 ;;
  
      * )
        usage
        exit 1
  
    esac
  
  done
  
  shift $((OPTIND - 1))

  if [[ -v $1 && $1 != "test" ]]; then
    usage
    exit 1
  fi

  if (( f == 1 )); then
    args="--rerun-failed --output-on-failure"
  fi

  pushd build
  ctest $args
  popd
}

tidy_task()
{
  local f args

  while getopts ":f" opt; do
  
    case $opt in 
  
      f ) f=1 ;;
  
      * )
        usage
        exit 1
  
    esac
  
  done
  
  shift $((OPTIND - 1))

  if [[ -v $1 && $1 != "tidy" ]]; then
    usage
    exit 1
  fi

  args='-quiet'

  if (( f == 1 )); then
    args="$args -fix"
  fi

  find src tests apps \
    -type f \( -iname "*.cpp" -o -iname "*.hpp" \) \
    -print0 \
    | xargs -0 -P 4 -n 1 run-clang-tidy $args

  #   files+=("$file")  # Accumulate files in an array
  # done < <(find src tests \
  #     -type f \( -iname "*.cpp" -o -iname "*.hpp" \) \
  #     -print0)
  #
  # printf "%s\n" "${files[@]}" | xargs -I{} -P 4 run-clang-tidy $args {}
}

format_task()
{
  if [[ -v $1 && $1 != "format" ]]; then
    usage
    exit 1
  fi

  find src tests apps \
    -type f \( -iname '*.cpp' -o -iname '*.hpp' \) \
    -print0 \
    | xargs -0 -P 4 -n 1 clang-format -i
}

# Get the task
task="${!#}"

if [[ $task == -* || $task == "$0" ]]; then
  # no task so default it
  task=build
fi

case $task in

  build ) build_task "$@" ;;

  test ) test_task "$@" ;;

  tidy ) tidy_task "$@" ;;
    
  format ) format_task "$@" ;;

  clean ) clean_task "$@" ;;

  * )
    usage
    exit 1

esac
