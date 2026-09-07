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

 Date: 02/07/2021 16:40:25
*/


-- ----------------------------
-- Table structure for car
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_car";
CREATE TABLE "public"."car" (
  "code" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "speed" float8,
  "gps" json,
  "create_date" timestamp(0),
  "update_date" timestamp(0),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "type" int8,
  "time" json
)
;

-- ----------------------------
-- Primary Key structure for table car
-- ----------------------------
ALTER TABLE "public"."car" ADD CONSTRAINT "ebike_car_pkey" PRIMARY KEY ("code");
