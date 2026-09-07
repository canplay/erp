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

 Date: 02/07/2021 16:40:11
*/


-- ----------------------------
-- Table structure for options
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_options";
CREATE TABLE "public"."options" (
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "options" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "level" int8
)
;

-- ----------------------------
-- Records of options
-- ----------------------------
INSERT INTO "public"."options" VALUES ('默认', '{"system":120,"alert":300}', '2021-06-16 22:15:00', '2021-06-16 22:15:02', 'f', 0);
