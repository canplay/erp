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

 Date: 02/07/2021 16:40:40
*/


-- ----------------------------
-- Table structure for storage_history_2021_6
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_storage_history_2021_6";
CREATE TABLE "public"."storage_history_2021_6" (
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "gps" json,
  "create_date" timestamp(6),
  "update_date" timestamp(6),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "sum" int8,
  "cur" int8,
  "points" varchar(255) COLLATE "pg_catalog"."default",
  "type" int8
)
;

-- ----------------------------
-- Primary Key structure for table storage_history_2021_6
-- ----------------------------
ALTER TABLE "public"."storage_history_2021_6" ADD CONSTRAINT "history_2021_6_copy1_pkey" PRIMARY KEY ("hash");
