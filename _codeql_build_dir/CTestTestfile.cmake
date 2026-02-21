# CMake generated Testfile for 
# Source directory: /home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine
# Build directory: /home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[telemetry_io]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/test_telemetry")
set_tests_properties([=[telemetry_io]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;117;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[golden_state]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/test_golden")
set_tests_properties([=[golden_state]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;125;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[book_snapshot]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/test_book_snapshot")
set_tests_properties([=[book_snapshot]=] PROPERTIES  WORKING_DIRECTORY "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir" _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;135;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[replay_help]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/replay" "--help")
set_tests_properties([=[replay_help]=] PROPERTIES  PASS_REGULAR_EXPRESSION "Blanc LOB Engine" _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;141;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[replay_default_run]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/replay")
set_tests_properties([=[replay_default_run]=] PROPERTIES  WORKING_DIRECTORY "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine" _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;143;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[tail_latency_purity]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/test_tail_latency")
set_tests_properties([=[tail_latency_purity]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;152;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
add_test([=[gate_transition_determinism]=] "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/_codeql_build_dir/bin/test_gate_transitions")
set_tests_properties([=[gate_transition_determinism]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;165;add_test;/home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/CMakeLists.txt;0;")
subdirs("bench")
