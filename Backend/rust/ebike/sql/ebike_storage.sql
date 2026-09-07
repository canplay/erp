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

 Date: 02/07/2021 16:40:33
*/


-- ----------------------------
-- Table structure for storage
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_storage";
CREATE TABLE "public"."storage" (
  "code" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "gps" json,
  "create_date" timestamp(0),
  "update_date" timestamp(0),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "sum" int8,
  "cur" int8,
  "status" int8,
  "points" varchar(5000) COLLATE "pg_catalog"."default",
  "type" int8
)
;

-- ----------------------------
-- Primary Key structure for table storage
-- ----------------------------
ALTER TABLE "public"."storage" ADD CONSTRAINT "ebike_storage_pkey" PRIMARY KEY ("code");
