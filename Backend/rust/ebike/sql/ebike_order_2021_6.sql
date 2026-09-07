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

 Date: 02/07/2021 16:39:59
*/


-- ----------------------------
-- Table structure for order_2021_6
-- ----------------------------
DROP TABLE IF EXISTS "public"."ebike_order_2021_6";
CREATE TABLE "public"."order_2021_6" (
  "code" varchar(255) COLLATE "pg_catalog"."default",
  "status" int8,
  "provide" varchar(255) COLLATE "pg_catalog"."default",
  "speed" float8,
  "gps" json,
  "time" json,
  "create_date" timestamp(0),
  "update_date" timestamp(0),
  "delete" bool,
  "alert" varchar(255) COLLATE "pg_catalog"."default",
  "remark" varchar(255) COLLATE "pg_catalog"."default",
  "type" int8,
  "hash" varchar(255) COLLATE "pg_catalog"."default" NOT NULL,
  "payable" float8,
  "pay" float8,
  "refund" float8,
  "coupon" float8,
  "order" varchar(255) COLLATE "pg_catalog"."default",
  "paytype" int8,
  "paytime" timestamp(0),
  "paystatus" int8
)
;

-- ----------------------------
-- Primary Key structure for table order_2021_6
-- ----------------------------
ALTER TABLE "public"."order_2021_6" ADD CONSTRAINT "car_copy1_pkey" PRIMARY KEY ("hash");
