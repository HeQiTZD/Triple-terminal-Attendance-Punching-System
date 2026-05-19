#ifndef PROTOCOL_H
#define PROTOCOL_H

// ============================================================
// DEPRECATED — 已被 ServerProtocol 替代（改造阶段 3.1.1）
// 请使用 NetworkClient/serverprotocol.h 中的 ServerProtocol 命名空间。
// 此文件将在阶段四删除。
// ============================================================

#include "serverprotocol.h"

// 向后兼容：Protocol 命名空间保留为 ServerProtocol 的别名
// 新代码请直接使用 ServerProtocol::
namespace Protocol {
    using ServerProtocol::PersonData;
    using ServerProtocol::AttendanceRecord;
}

#endif // PROTOCOL_H
