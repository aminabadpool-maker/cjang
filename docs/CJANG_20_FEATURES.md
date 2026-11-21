# CJANG 20-Feature Roadmap (C11 Fixed-Buffer Architecture)

## 1. Persistent Database & User Comments (1-5)
1. Fixed-buffer disk persistence engine (`CJCMT01` header + binary records)
2. Add user comment endpoint (`POST /comment`) with `note_id`, `author`, `text`
3. List comments endpoint (`GET /comments`) filtered by `note_id`
4. Comment moderation (`POST /comment/moderate` with `approved` / `hidden` / `pending`)
5. Comment deletion & note comment counters (`POST /comment/delete`, count helpers)

## 2. Notes & Content Management (6-10)
6. Unique ID note CRUD (`create`, `read`, `update`, `delete`)
7. Tags and categories on notes
8. Full-text search and filtering by tag or status
9. Offset/limit pagination across notes and comments
10. Database backup export and import/restore

## 3. Users, Auth & Security Guardrails (11-15)
11. Session tokens and cookie authentication
12. Role separation (`guest` / `user` / `moderator` / `admin`)
13. Strict input validation and bounded fixed buffers
14. Rate limiting per client/session against comment spam
15. Safe JSON and HTML escaping for user-generated content

## 4. HTTP Engine, CLI & Operations (16-20)
16. Exact `Content-Length` body extraction and validation
17. Structured logging with log levels (`INFO`, `WARN`, `ERROR`)
18. Real health and runtime metrics endpoints (`/health`, `/metrics`)
19. Expanded `cjang_bridge_cli` commands for comments, notes, and DB files
20. End-to-end integration tests covering persistence across restarts
