/*
 Navicat Premium Data Transfer

 Source Server         : 127.0.0.1_PostgreSQL
 Source Server Type    : PostgreSQL
 Source Server Version : 130002
 Source Host           : localhost:5432
 Source Catalog        : ebike
 Source Schema         : public

 Target Server Type    : PostgreSQL
 Target Server Version : 130002
 File Encoding         : 65001

 Date: 02/07/2021 16:40:47
*/


-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_user";
CREATE TABLE "public"."user" (
  "username" varchar(255) COLLATE "pg_catalog"."default",
  "password" varchar(255) COLLATE "pg_catalog"."default",
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "status" int8,
  "last_login" timestamp(6),
  "level" int8
)
;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO "public"."user" VALUES ('admin', 'CHANGE_ME_ADMIN_PASSWORD', '2021-06-08 10:34:58', '2021-06-08 10:35:03', 'f', 1, '2021-06-28 16:03:52.663783', 0);
