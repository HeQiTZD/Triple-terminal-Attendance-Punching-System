-- ============================================================
-- 001_initial.sql — Schema v1: 全新安装建表
-- 注意: 设备端不存储人员个人信息，仅存储人脸特征用于识别
-- ============================================================

-- 人脸特征表（仅用于人脸识别匹配，不关联个人信息）
CREATE TABLE IF NOT EXISTS face_feature (
    employee_id     TEXT NOT NULL PRIMARY KEY,
    feature_blob    BLOB NOT NULL,
    feature_size    INTEGER NOT NULL,
    updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    sync_generation INTEGER NOT NULL DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_facefeature_sync_gen ON face_feature(sync_generation);

-- 离线考勤出箱表
CREATE TABLE IF NOT EXISTS attendance_outbox (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    client_msg_id  TEXT NOT NULL UNIQUE,
    employee_id    TEXT NOT NULL,
    check_time     DATETIME NOT NULL,
    status         TEXT NOT NULL DEFAULT 'ok',
    photo_blob     BLOB,
    photo_size     INTEGER DEFAULT 0,
    created_at     DATETIME DEFAULT CURRENT_TIMESTAMP,
    retry_count    INTEGER NOT NULL DEFAULT 0,
    last_error     TEXT,
    state          TEXT NOT NULL DEFAULT 'pending'
        CHECK (state IN ('pending','sending','failed','dead'))
);
CREATE INDEX IF NOT EXISTS idx_outbox_state ON attendance_outbox(state);
CREATE INDEX IF NOT EXISTS idx_outbox_employee_id ON attendance_outbox(employee_id);

-- 同步元数据表（单行）
CREATE TABLE IF NOT EXISTS sync_meta (
    id                       INTEGER PRIMARY KEY CHECK (id = 1),
    current_generation       INTEGER NOT NULL DEFAULT 1,
    staging_generation       INTEGER NOT NULL DEFAULT 0,
    last_sync_request_msg_id TEXT,
    last_sync_ok_at          DATETIME,
    last_sync_status         TEXT,
    face_count               INTEGER DEFAULT 0
);

-- 本机信息表（单行）
CREATE TABLE IF NOT EXISTS device_local (
    id          INTEGER PRIMARY KEY CHECK (id = 1),
    device_id   TEXT NOT NULL,
    device_name TEXT NOT NULL DEFAULT '',
    ip_address  TEXT NOT NULL DEFAULT '',
    fw_version  TEXT NOT NULL DEFAULT '1.0.0'
);

-- Schema 版本表
CREATE TABLE IF NOT EXISTS schema_version (
    version    INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

INSERT OR REPLACE INTO schema_version (version) VALUES (1);
