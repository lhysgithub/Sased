# Sased

Sased是一款针对Linux内核的语义感知的安全检查检测工具。

Sased的编码是基于LLVM 10.0.0，其环境配置参考http://llvm.org/docs/GettingStarted.html。

## 编译

```bash
# 构建Makefile
cmake ./
# 编译Sased
make
```

## 运行

```bash
cd test
# 统一运行
python3 test.py target.bc
##############################
# 分模块运行
# 运行基于自然语义检测模块
opt -load ../pass1/libPass1.so -newcg target.bc
# 运行基于程序语义检测模块
opt -load ../pass2/libPass2.so -newcg target.bc
# 运行回溯过滤模块
opt -load ../pass3/libPass3.so -newcg target.bc
# 运行安全检查检测模块
opt -load ../pass4/libPass4.so -newcg target.bc
```

## 发现的漏洞

经过人工确认，我们发现Linux kernel 5.5-rc6 中存在以下漏洞：

| 文件/行号                           | 函数                                     | 变量          | 缺失检查类型                                    | 漏洞类型     |
| ----------------------------------- | ---------------------------------------- | ------------- | ----------------------------------------------- | ------------ |
| /arch/x86/xen/p2m.c 232行           | xen_build_mfn_list_list                  | p2m_top_mfn_p | 返回值 alloc_p2m_page                           | 空指针解引用 |
| /arch/x86/xen/p2m.c 235行           | xen_build_mfn_list_list                  | p2m_top_mfn   | 返回值 alloc_p2m_page                           | 空指针解引用 |
| /drivers/usb/host/xhci-pci.c 276行  | xhci_pme_acpi_rtd3_enable                | obj           | 返回值 acpi_evaluate_dsm                        | 释放空指针   |
| /lib/dynamic_debug.c 1066行         | dynamic_debug_init                       | cmdline       | 返回值 kstrdup                                  | 释放空指针   |
| /mm/page_alloc.c 6174行             | setup_zone_pageset                       | zone          | 返回值 alloc_percpu                             | 空指针解引用 |
| /kernel/trace/ring_buffer.c  1959行 | rb_inc_iter                              | head_page     | 返回值 rb_set_head_page                         | 空指针解引用 |
| /kernel/bpf/btf.c 2082行            | btf_array_seq_show                       | elem_type     | 返回值 btf_type_id_size                         | 空指针解引用 |
| /fs/ecryptfs/keystore.c 1663行      | decrypt_passphrase_encrypted_session_key | tfm           | 参数 ecryptfs_get_tfm_and_mutex_for_cipher_name | 空指针解引用 |
| /drivers/usb/host/xhci.c 3216行     | xhci_endpoint_reset                      | ctrl_ctx      | 返回值 xhci_get_input_control_ctx               | 空指针解引用 |

