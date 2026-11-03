# R50 Windows 候选

以R48为基线，增加通用的组合LUT输入排列规范化。九例和原压力资源保持，重点减少重复映射模板及内存负担。

根目录yosys.exe、yosys-abc.exe和share为运行依赖，不依赖原项目绝对路径或WSL。

运行复验（Python3.9+）：`python verify_portable.py --out ../运行复验`。仅校验文件用`--check-only`。本入口重跑十例并核对两检查点网表和资源，不是独立功能证明或效率评分。

独立功能复验：`python verify_functional.py --yosys <OSS-CAD-Suite/bin/yosys.exe> --out ../功能复验`。需要合同中固定哈希的独立工具及对应bin/lib。会重新综合当前exe、重新生成并验证状态方程、直接对照两套原压力参考，并对36个显式初值/组合RTL用例执行完整归纳证明。它不宣称覆盖未定义初值RTL的所有解释或任意X/Z。

从源码重新生成并编译：`python source/platform/build_portable.py --llvm <LLVM-MinGW根目录> --out ../全新构建`。Python、LLVM-MinGW 23.1.4为显式构建依赖；CMake/Ninja/winflexbison已随源码携带。入口新建临时源码树，从配置、生成到160对象全部重建，不读冻结解析器或旧对象。Clang ABI自动检测在本机异常退出，因此使用已实编验证的编译器事实；入口实际编译C++20探针并保留配置日志。构建日期、来源路径可使二进制哈希不同，需另行核对功能网表并独立测速。

正式效率结果与两组预先规定的稳定性观察全部保留在evidence/efficiency。3%是内部目标，正式门槛为5%；某次通过不保证其它环境通过。历史增量构建、冻结生成物重编、完整重生成是不同哈希，不混用成绩。

完整结论与适用边界见evidence/最终执行报告.md。r48-base保留旧版原始证据以支持验证链，不代表本包仍采用R48源码。原版fresh_08默认BRAM失败单列，-nobram补充验证不冒充默认流程通过。离散二值证明不覆盖物理时序、模拟上电或任意未知值。

本包内容与正式提交目录一致。未知赛方用例的满分不能由有限测试保证。

重建后可运行 `python verify_portable.py --program ../全新构建/program/yosys.exe --out ../重建网表复验` 核对十例两检查点；它会记录实际指定程序的哈希，不能把原候选效率结果转移给另一产物。
ABC9 面积脚本是编译进程序的内置字符串，位于 source/passes/techmap/abc9.cc 的 constpad，全流程统一使用同一串：
+&sweep; &dc2; &dch -f -r -s; &ps; &if {W} {D} {R} -a -v。脚本不随用例或结构切换。

等价性由随包证明脚本判定：映射网表还原为显式布尔方程与状态转移关系，RTL 归纳使用每个用例保存的受限原语模型 primitive_decode.v；不依赖 EQY，也不读综合程序的内部状态。
验证脚本已禁用 Python 字节码写入（sys.dont_write_bytecode），在包内直接运行也不会改动包内任何文件；仍建议解压到新目录复验，以免输出目录与交付物混在一起。综合程序本身只读包内相对路径，不向工作目录写入产物。